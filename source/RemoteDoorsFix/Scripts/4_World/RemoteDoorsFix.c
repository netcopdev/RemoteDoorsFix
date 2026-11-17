// RemoteDoors 1.30.0 compatibility layer. No upstream assets are bundled.
class RDF_GroupControl
{
    // Control characters cannot be part of an upstream stored group name.
    static const string ALL = "\n";
    static const string CLOSE = "\t";

    static bool Operate(PlayerBase player, string groupName, bool all, bool closeOnly, out int changed, out int already, out int failed, out bool opening, out bool limitReached, array<ref RD_ServerTarget> pending)
    {
        changed = 0;
        already = 0;
        failed = 0;
        opening = false;
        limitReached = false;

        array<ref RD_ServerTarget> targets;
        string filter = groupName;
        if (all) filter = "";
        RD_ScanService.Collect(player, targets, filter);
        if (!targets || targets.Count() == 0) return false;

        bool anyOpen = false;
        foreach (RD_ServerTarget target: targets)
        {
            if (target && target.IsOpen) anyOpen = true;
        }

        limitReached = targets.Count() >= RD_SettingsService.Get().MaximumTargetsPerScan;
        // A capped scan cannot prove that every member of a group is closed.
        // Never open on incomplete evidence, a close-only request or mixed state.
        opening = !all && !closeOnly && !anyOpen && !limitReached;
        if (limitReached && !all && !closeOnly && !anyOpen) return true;

        foreach (RD_ServerTarget item: targets)
        {
            if (!item) continue;
            if (item.IsOpen == opening)
            {
                already++;
                continue;
            }
            int result = RD_TargetService.SetOpen(item, opening);
            if (result == RD_Result.CHANGED) changed++;
            else if (result == RD_Result.ALREADY) already++;
            else if (result == RD_Result.PENDING) pending.Insert(item);
            else failed++;
        }
        return true;
    }
}

modded class RD_ClientState
{
    // Keep the global action outside the upstream array; real names and the
    // upstream RPC group-list format remain unchanged.
    protected static bool s_RDFGlobal = true;

    static override void ResetScan()
    {
        super.ResetScan();
        s_RDFGlobal = true;
    }

    static override bool CanCycleGroups()
    {
        return s_Groups.Count() > 0;
    }

    static override void Next()
    {
        if (s_Groups.Count() == 0)
        {
            s_RDFGlobal = true;
            return;
        }
        if (s_RDFGlobal)
        {
            s_RDFGlobal = false;
            s_SelectedIndex = 0;
        }
        else if (s_SelectedIndex >= s_Groups.Count() - 1)
        {
            s_RDFGlobal = true;
        }
        else s_SelectedIndex++;
    }

    static override void Previous()
    {
        if (s_Groups.Count() == 0)
        {
            s_RDFGlobal = true;
            return;
        }
        if (s_RDFGlobal)
        {
            s_RDFGlobal = false;
            s_SelectedIndex = s_Groups.Count() - 1;
        }
        else if (s_SelectedIndex <= 0)
        {
            s_RDFGlobal = true;
        }
        else s_SelectedIndex--;
    }

    static override string SelectionLabel()
    {
        if (s_Groups.Count() == 0)
        {
            s_RDFGlobal = true;
            return "No named doors in range";
        }
        if (s_RDFGlobal)
        {
            foreach (RD_GroupSummary group: s_Groups)
            {
                if (group && group.OpenCount > 0)
                    return string.Format("1/%1  Close All", s_Groups.Count() + 1);
            }
            return string.Format("1/%1  All Secure (last scan)", s_Groups.Count() + 1);
        }
        if (s_SelectedIndex < 0 || s_SelectedIndex >= s_Groups.Count()) return "No selected group";
        RD_GroupSummary selected = s_Groups.Get(s_SelectedIndex);
        string action = "Open ";
        if (selected.OpenCount > 0) action = "Close ";
        return string.Format("%1/%2  %3%4 (%5 doors)", s_SelectedIndex + 2, s_Groups.Count() + 1, action, selected.Name, selected.Total);
    }

    static override void ActivateSelected()
    {
        if (!IsPowered() || s_Groups.Count() == 0) return;
        string request = RDF_GroupControl.ALL;
        if (!s_RDFGlobal)
        {
            if (s_SelectedIndex < 0 || s_SelectedIndex >= s_Groups.Count()) return;
            RD_GroupSummary selected = s_Groups.Get(s_SelectedIndex);
            request = selected.Name;
            // A displayed Close command must not turn into Open if the
            // server state changes between the scan and activation.
            if (selected.OpenCount > 0) request = RDF_GroupControl.CLOSE + request;
        }
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(request);
        Send(RD_RPC.C2S_ACTIVATE, rpc);
    }
}

modded class RD_Network
{
    // Existing HandleServer still owns authentication, powered-remote checks
    // and cooldown. Only the activation policy is replaced.
    protected static override void HandleActivate(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
    {
        string request;
        if (!ctx.Read(request)) return;
        bool all = request == RDF_GroupControl.ALL;
        bool closeOnly = all;
        string groupName = request;
        if (!all)
        {
            if (request.Length() > 0 && request.Substring(0, 1) == RDF_GroupControl.CLOSE)
            {
                closeOnly = true;
                groupName = request.Substring(1, request.Length() - 1);
            }
            if (!RD_Text.IsValidGroupName(groupName)) return;
        }

        int changed;
        int already;
        int failed;
        bool opening;
        bool limited;
        array<ref RD_ServerTarget> pending = new array<ref RD_ServerTarget>();
        string label = groupName;
        if (all) label = "all scanned doors";

        if (!RDF_GroupControl.Operate(player, groupName, all, closeOnly, changed, already, failed, opening, limited, pending))
        {
            SendMessage(player, sender, "No authorized targets remain for " + label + ".");
            SendGroups(player, sender);
            return;
        }
        if (limited)
        {
            SendMessage(player, sender, "Scan limit reached. Only scanned targets are covered; incomplete groups cannot be opened.");
        }
        if (pending.Count() > 0)
        {
            RD_PendingDoorOperation operation = new RD_PendingDoorOperation();
            operation.Player = player;
            operation.Sender = sender;
            operation.GroupName = label;
            operation.Opening = opening;
            operation.Changed = changed;
            operation.Already = already;
            operation.Failed = failed;
            operation.Targets = pending;
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VerifyBuildingDoors, 250, false, operation);
            return;
        }
        SendOperationResult(player, sender, label, opening, changed, already, failed);
    }
}

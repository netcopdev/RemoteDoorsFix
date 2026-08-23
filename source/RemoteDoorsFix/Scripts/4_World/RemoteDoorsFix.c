/*
    Compatibility patch for RemoteDoors (Workshop 2946892641).

    Features:
    - Stable alphabetical target indexing on client and server.
    - Duplicate/null Fence reference cleanup.
    - Security-first set behavior:
        ANY open member => Close <Set>
        ALL closed      => Open <Set>
    - Synthetic first action:
        Close All  when any known target is open
        All Secure when every known target is closed

    No automatic distance closing or geofence logic.
*/

modded class CodeLockRemote
{
    ref TStringArray m_RDFSortedNames;

    void RDF_NormalizeTargets()
    {
        if (!m_RDFSortedNames)
            m_RDFSortedNames = new TStringArray;

        m_RDFSortedNames.Clear();

        if (!scara_allowedmulti)
            return;

        int mapCount = scara_allowedmulti.Count();

        for (int mapIndex = 0; mapIndex < mapCount; mapIndex++)
        {
            string targetName = scara_allowedmulti.GetKey(mapIndex);
            array<Fence> fences = scara_allowedmulti.GetElement(mapIndex);

            if (!fences)
                continue;

            for (int i = fences.Count() - 1; i >= 0; i--)
            {
                Fence currentFence = fences.Get(i);

                if (!currentFence)
                {
                    fences.Remove(i);
                    continue;
                }

                for (int j = 0; j < i; j++)
                {
                    if (fences.Get(j) == currentFence)
                    {
                        fences.Remove(i);
                        break;
                    }
                }
            }

            if (targetName != "" && fences.Count() > 0)
                m_RDFSortedNames.Insert(targetName);
        }

        m_RDFSortedNames.Sort();
    }

    string RDF_GetTargetName(int targetIndex)
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames)
            return "";

        if (targetIndex < 0 || targetIndex >= m_RDFSortedNames.Count())
            return "";

        return m_RDFSortedNames.Get(targetIndex);
    }

    bool RDF_HasAnyOpen()
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames || !scara_allowedmulti)
            return false;

        for (int nameIndex = 0; nameIndex < m_RDFSortedNames.Count(); nameIndex++)
        {
            string targetName = m_RDFSortedNames.Get(nameIndex);
            array<Fence> fences = scara_allowedmulti.Get(targetName);

            if (!fences)
                continue;

            for (int fenceIndex = 0; fenceIndex < fences.Count(); fenceIndex++)
            {
                Fence currentFence = fences.Get(fenceIndex);

                if (currentFence && currentFence.IsOpened())
                    return true;
            }
        }

        return false;
    }

    void RDF_CloseAll()
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames || !scara_allowedmulti)
            return;

        for (int nameIndex = 0; nameIndex < m_RDFSortedNames.Count(); nameIndex++)
        {
            string targetName = m_RDFSortedNames.Get(nameIndex);
            array<Fence> fences = scara_allowedmulti.Get(targetName);

            if (!fences)
                continue;

            for (int fenceIndex = 0; fenceIndex < fences.Count(); fenceIndex++)
            {
                Fence currentFence = fences.Get(fenceIndex);

                if (currentFence && currentFence.IsOpened())
                    currentFence.CloseFence();
            }
        }
    }

    override int GetLocks()
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames || m_RDFSortedNames.Count() == 0)
            return 0;

        // Reserve index 0 permanently so real target indexes never shift
        // when open/closed state changes.
        return m_RDFSortedNames.Count() + 1;
    }

    override string GetCurrentIndexName(int index)
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames || m_RDFSortedNames.Count() == 0)
            return "Unknown";

        if (index == 0)
        {
            if (RDF_HasAnyOpen())
                return "Close All";

            return "All Secure";
        }

        string targetName = RDF_GetTargetName(index - 1);

        if (targetName == "")
            return "Unknown";

        if (!scara_allowedmulti)
            return "Unknown";

        array<Fence> fences = scara_allowedmulti.Get(targetName);

        if (!fences || fences.Count() == 0)
            return "Unknown";

        bool anyOpen = false;
        int validCount = 0;

        for (int i = 0; i < fences.Count(); i++)
        {
            Fence currentFence = fences.Get(i);

            if (!currentFence)
                continue;

            validCount++;

            if (currentFence.IsOpened())
                anyOpen = true;
        }

        if (validCount == 0)
            return "Unknown";

        string output = targetName;

        if (validCount > 1)
            output = output + " [Set]";

        if (anyOpen)
            return string.Format("Close %1", output);

        return string.Format("Open %1", output);
    }

    override void OnWork(float consumed_energy)
    {
        super.OnWork(consumed_energy);
        RDF_NormalizeTargets();
    }

    override void OnWorkStop()
    {
        super.OnWorkStop();

        if (m_RDFSortedNames)
            m_RDFSortedNames.Clear();
    }

    override void OnApply(PlayerBase player)
    {
        RDF_NormalizeTargets();

        if (!m_RDFSortedNames || m_RDFSortedNames.Count() == 0)
            return;

        if (scara_lockIndex == 0)
        {
            RDF_CloseAll();
            return;
        }

        string targetName = RDF_GetTargetName(scara_lockIndex - 1);

        if (targetName == "")
            return;

        if (!scara_allowedmulti)
            return;

        array<Fence> fences = scara_allowedmulti.Get(targetName);

        if (!fences || fences.Count() == 0)
            return;

        bool anyOpen = false;
        int validCount = 0;

        for (int i = 0; i < fences.Count(); i++)
        {
            Fence currentFence = fences.Get(i);

            if (!currentFence)
                continue;

            validCount++;

            if (currentFence.IsOpened())
                anyOpen = true;
        }

        if (validCount == 0)
            return;

        for (int j = 0; j < fences.Count(); j++)
        {
            Fence targetFence = fences.Get(j);

            if (!targetFence)
                continue;

            if (anyOpen)
            {
                if (targetFence.IsOpened())
                    targetFence.CloseFence();
            }
            else
            {
                if (!targetFence.IsOpened())
                    targetFence.OpenFence();
            }
        }
    }
}

# RemoteDoors 1.30.0 qualification

Reference: issue #1. Upstream PBO SHA-256: 1407dc3ae7a4789cd8603f0e7f206d95c4009b9996205d8051e49c045133ac51.

## Required runtime checks (not yet executed)

- Build/sign version 0.2.0; verify exact PBO hash after deployment.
- Back up mission persistence and profiles (including RemoteDoors metadata).
- Server World module compiles; client World and Mission modules compile; no startup crash attributable to the patch.
- Existing authorized named targets remain visible; no changes to upstream item class IDs or storage schema by the patch.
- Alphabetical real groups, first global entry, single-group and multiple-group cycling in both directions, scan refresh and power cycle.
- All closed: group command opens; all open: closes; mixed: only closes.
- Select a displayed Close action, have another actor close the group, then activate: remains closed.
- Close All: closes only currently authorized open targets, never opens anything. All Secure activation also never opens.
- Unnamed/out-of-range/unauthorized targets unaffected. Invalid RPC names, absent or unpowered remote, cooldown and authentication retain upstream behavior.
- Community gates/tents and vanilla gates/buildings including delayed building lock/animation completion and failures.
- With MaximumTargetsPerScan reached, warning is shown and no incomplete group can be opened. Close All covers only the bounded scan.
- Client/server both use the same patch version. Old controller items/group names may need upstream migration, separate from this patch.

AddonBuilder packaging success alone is not a script compiler qualification.

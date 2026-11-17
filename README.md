# RemoteDoorsFix

Compatibility and close-first safety patch for [RemoteDoors](https://steamcommunity.com/sharedfiles/filedetails/?id=2946892641).

## Compatibility

Version 0.2.x targets the rewritten RemoteDoors 1.30.0 API (upstream PBO SHA-256 `1407dc3ae7a4789cd8603f0e7f206d95c4009b9996205d8051e49c045133ac51`). The original CodeLockRemote API is no longer supported on this branch. Upstream already provides sorted named groups and deduplicated targets.

## Behavior

- Global first selection: **Close All** if any scanned member is open, otherwise **All Secure (last scan)**. Activating it is always close-only.
- Individual group: close if any member is open, open only if every freshly scanned member is closed.
- A client selection that displays Close sends close-only intent; it never opens a group even if its state has changed.
- Operations are limited to currently authorized, named targets within upstream scan range and target limits. A capped scan cannot authorize opening a group. All Secure describes the last scan, not the whole map.
- Retains upstream target handling for community Code Lock gates/tents and vanilla Digital Code Lock gates/buildings, authentication, held-remote/battery checks, cooldown, and pending building animation verification.
- No automatic closing, persistence conversion, or upstream asset repacking.

## Requirements

Load Code Lock, RemoteDoors 1.30.0, then RemoteDoorsFix on server and client. The patch is a regular client/server mod. Both sides must use the same patch version.

```text
...;@CodeLock;@RemoteDoors;@RemoteDoorsFix;...
```

Use the upstream RemoteDoors naming action to name targets; the new version stores group names independently of Code Lock public names. No automatic migration of old names or remote items is provided.

## Qualification

Candidate: source/API inspection only until DayZ compilation and functional tests pass. See [validation checklist](docs/qualification.md).

## License

MIT for the original patch code only. RemoteDoors, Code Lock, DayZ and other third-party dependencies retain their own licenses and terms. No upstream PBO or assets are distributed.

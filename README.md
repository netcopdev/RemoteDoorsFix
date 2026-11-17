# RemoteDoorsFix

RemoteDoorsFix is a compatibility and safety patch for [RemoteDoors](https://steamcommunity.com/sharedfiles/filedetails/?id=2946892641).

RemoteDoors allows a handheld controller to operate doors through their Code Lock public names. This patch makes the controller menu more predictable and ensures that closing doors is always prioritized when a target contains multiple doors in different states.

## What it does

- Sorts named targets alphabetically so the menu remains consistent.
- Removes duplicate and missing door references.
- Keeps the global safety status in the first menu position, showing **Close All** whenever any known door is open.
- Shows **All Secure** when every known door is closed.
- Groups doors with the same Code Lock public name into a single `[Set]` entry.
- Shows **Close** for a set when at least one door in that set is open.
- Closes every open door in a mixed set instead of opening the closed doors.
- Shows **Open** only when every door in the selected set is already closed.

The **Close All** option is safety-only: it closes open doors and never opens closed ones. This patch does not automatically close doors based on distance or player movement.

## Requirements

- Code Lock
- RemoteDoors (upstream `CfgPatches` addon identifier: `RemoteDoors`)
- RemoteDoorsFix loaded after RemoteDoors

RemoteDoorsFix must be loaded by both the client and server. It is a regular client/server mod, not a `serverMod`.

```text
...;@CodeLock;@RemoteDoors;@RemoteDoorsFix;...
```

The patch declares an explicit engine dependency on `RemoteDoors`, as verified in the deployed upstream 1.30.0 PBO. Keep the command-line load order above as well. Compatibility of the upstream controller script API with the patch must be verified on DayZ before deployment; the addon dependency alone does not establish runtime compatibility.

## Using door sets

Give multiple Code Locks the exact same public name to control their doors as one set. If any door in that set is open, the controller offers to close the set. Only a completely closed set can be opened.

The original RemoteDoors mod and Code Lock are still required. RemoteDoorsFix does not contain or repack their assets.

## License

RemoteDoorsFix is licensed under the MIT License. See [LICENSE](LICENSE).

This license applies only to the original RemoteDoorsFix source code and assets.
RemoteDoors, Code Lock, DayZ, and any other third-party dependencies remain
subject to their respective licenses and terms.
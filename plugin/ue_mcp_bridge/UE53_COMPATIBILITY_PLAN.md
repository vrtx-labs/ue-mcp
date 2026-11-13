# UE 5.3 Compatibility

## State

The plugin's one editor module, `UE_MCP_Bridge`, builds, links and loads on
UE 5.3.2. There is no separate 5.3 module and no 5.3-specific plugin
descriptor: the same sources and the same `.uplugin` serve 5.3 through 5.8.

On 5.3.2 the bridge registers 1006 of the 1054 native actions. The 48 that
are absent are the StateTree actions, which need UE 5.5 on every engine.

## How 5.3 differences are handled

- **Version gates.** `UE_MCP_HAS_5_4_API` (in `Public/HandlerUtils.h`) is
  false on 5.3 and gates each API that 5.4 introduced. Shims shared by more
  than one file live in that header.
- **Moved headers.** `Public/MCPEngineCompat.h` covers the include paths, and
  aliases `EMovieSceneTimeUnit` to 5.3's `ESequenceTimeUnit`.
- **Actions 5.3 cannot perform.** They stay registered and answer with the
  engine requirement, through `MCP_HANDLER_NEEDS_ENGINE`, instead of
  "Unknown method".
- **Unexported classes.** The BehaviorTree graph node classes do not link on
  5.3, so BehaviorTree authoring resolves them by class path.
- **Descriptor.** `PropertyBindingUtils` does not exist on 5.3. It is an
  optional plugin dependency and `Build.cs` links it from 5.4 up.

## What needs UE 5.4 or newer

These answer with a "needs UE 5.4 or newer" error on 5.3:

- PoseSearch schema, normalization set and database authoring, and the
  motion matching and pose history AnimGraph nodes. The 5.3 PoseSearch plugin
  has a single-skeleton schema and a different database model.
- Chooser table authoring. The 5.3 Chooser plugin keeps its table header
  engine-internal and has no compile step or fallback result.
- `auto_align_retarget_pose`. `UIKRetargeterController` has no auto-align on 5.3.
- Skeletal mesh UV authoring (static mesh UVs work).
- `includeDependencies=false` on asset migration.

## What behaves differently

- Sequencer scrubbing moves by whole display-rate frames.
- `get_cvars` cannot report a console variable's default value.
- Trace pause state is what the bridge itself last applied; a pause made
  elsewhere is not visible.
- AI perception reports a sense's configured starting state, not its live
  per-component state, and omits `isFriendly`.
- Asset validation omits the requested count from the engine, per-asset
  message lists and the asset-limit flag, none of which 5.3 records.
- Duplicate detection in the asset hygiene audit skips the saved-package hash.
- Runtime virtual texture reads omit `expandBounds`.
- The BehaviorTree authoring automation tests start at 5.4.

## Building and verifying against 5.3

The 5.3 target is a real project, not the repository's test project:

```text
D:Unreal ProjectsVELUX_Design_StudioVELUX_Design_Studio.uproject
```

- Deploy with `npx tsx src/deploy-cli.ts <uproject>`; never hand-copy.
- Build the project's editor target with the 5.3 engine's `Build.bat`.
- Run read and validation actions only. Mutating coverage belongs in
  `tests/ue_mcp`, which has no 5.3 setup yet, so mutating actions are
  compiled on 5.3 but not exercised there.
- Confirm `project(get_status)` reports the expected editor before any live call.

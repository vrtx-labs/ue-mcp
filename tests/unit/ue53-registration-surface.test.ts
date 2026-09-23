/**
 * UE 5.3 deliberately builds a small bridge, not the complete modern plugin.
 *
 * The TypeScript graph remains version-neutral so clients can describe the
 * complete product.  The 5.3 module instead defines UE_MCP_BLUEPRINT53_CORE
 * and registers only handlers that compile against that engine.  This test
 * evaluates those source branches without an Unreal build, making an accidental
 * registrar expansion or a newly unreachable 5.3 handler visible in CI.
 */
import { readFileSync } from "node:fs";
import * as path from "node:path";
import { fileURLToPath } from "node:url";
import { describe, expect, it } from "vitest";
import { enumerateBridgeActions } from "../../src/tools.js";

const ROOT = path.join(path.dirname(fileURLToPath(import.meta.url)), "..", "..");
const BRIDGE = path.join(ROOT, "plugin", "ue_mcp_bridge", "Source", "UE_MCP_Bridge", "Private");

const REGISTRAR_SOURCES: Record<string, string> = {
  FBlueprintHandlers: "Handlers/BlueprintHandlers.cpp",
  FCollisionQueryHandlers: "Handlers/BlueprintHandlers_Collision.cpp",
  FDiffHandlers: "Handlers/DiffHandlers.cpp",
  FEditorHandlers: "Handlers/EditorHandlers.cpp",
};

// These actions add TypeScript policy before dispatch, so they intentionally
// do not expose a declarative `bridge` field in ALL_TOOLS.
const CUSTOM_ACTION_BRIDGES = new Set([
  "execute_python",
  "request_editor_shutdown",
]);

function read(relativePath: string): string {
  return readFileSync(path.join(BRIDGE, relativePath), "utf8");
}

/** Keep only the branches compiled by the UE 5.3 compatibility module. */
function ue53Source(source: string): string {
  const active: boolean[] = [true];
  const branchTaken: boolean[] = [];
  const output: string[] = [];

  for (const line of source.split(/\r?\n/)) {
    const directive = line.match(/^\s*#\s*(if|ifdef|ifndef|else|endif)\b\s*(.*)$/);
    if (!directive) {
      if (active.at(-1)) output.push(line);
      continue;
    }

    const [, kind, condition] = directive;
    if (kind === "if" || kind === "ifdef" || kind === "ifndef") {
      const named = condition.trim();
      let enabled = named === "UE_MCP_BLUEPRINT53_CORE";
      if (named === "UE_MCP_HAS_5_4_API") enabled = false;
      if (kind === "ifndef") enabled = !enabled;
      if (kind === "if" && named.startsWith("!")) enabled = !ue53Condition(named.slice(1));
      else if (kind === "if") enabled = ue53Condition(named);
      branchTaken.push(enabled);
      active.push(Boolean(active.at(-1)) && enabled);
      continue;
    }
    if (kind === "else") {
      const parent = active.length > 1 ? active[active.length - 2] : false;
      const previous = branchTaken.at(-1) ?? false;
      active[active.length - 1] = parent && !previous;
      branchTaken[branchTaken.length - 1] = true;
      continue;
    }
    active.pop();
    branchTaken.pop();
  }
  return output.join("\n");
}

function ue53Condition(condition: string): boolean {
  return condition.trim() === "UE_MCP_BLUEPRINT53_CORE";
}

function registeredMethods(source: string): string[] {
  return [...ue53Source(source).matchAll(/Registry\.RegisterHandler(?:WithTimeout)?\(\s*TEXT\("([a-z0-9_]+)"\)/g)]
    .map((match) => match[1])
    .sort();
}

function ue53Registrars(): string[] {
  const active = ue53Source(read("BridgeServer.cpp"));
  return [...active.matchAll(/\b(F\w+Handlers)::RegisterHandlers\(HandlerRegistry\)/g)]
    .map((match) => match[1])
    .sort();
}

describe("UE 5.3 registration surface", () => {
  it("keeps the compatibility module on its explicit four-registrar gate", () => {
    expect(ue53Registrars()).toEqual(Object.keys(REGISTRAR_SOURCES).sort());
  });

  it("registers only TypeScript-reachable handlers", () => {
    const registered = Object.entries(REGISTRAR_SOURCES)
      .flatMap(([registrar, file]) => {
        const methods = registeredMethods(read(file));
        expect(methods, `${registrar} parser found no handlers in ${file}`).not.toEqual([]);
        return methods;
      });
    const advertised = new Set([
      ...enumerateBridgeActions().map(({ bridge }) => bridge),
      ...CUSTOM_ACTION_BRIDGES,
    ]);
    const unreachable = [...new Set(registered)].filter((method) => !advertised.has(method));

    expect(unreachable, "every UE 5.3 registered handler needs an ALL_TOOLS bridge action").toEqual([]);
    expect(new Set(registered).size).toBeGreaterThan(100);
    expect(new Set(registered).size).toBeLessThan(advertised.size);
  });
});

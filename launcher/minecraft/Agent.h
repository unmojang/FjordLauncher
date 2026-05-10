#pragma once

#include <QString>
#include <map>
#include <string>
#include <unordered_set>

#include "Library.h"

static const std::unordered_set<std::string> MANAGED_AGENTS = { "moe.yushi:authlibinjector", "org.unmojang:Loki" };
static const std::map<std::string, std::string> AGENT_CLASS_TO_MANAGED_AGENT = {
    { "moe.yushi.authlibinjector.Premain", "moe.yushi:authlibinjector" },
    { "org.unmojang.loki.Loki", "org.unmojang:Loki" }
};

struct Agent {
    /// The library pointing to the jar this Java agent is contained within
    LibraryPtr library;

    /// The argument to the Java agent, passed after an = if present
    QString argument;
};

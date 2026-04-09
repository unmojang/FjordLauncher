#pragma once

#include <QString>
#include <unordered_set>

#include "Library.h"

static const std::unordered_set<std::string> MANAGED_AGENTS = { "moe.yushi:authlibinjector" };
static const std::map<std::string, std::string> AGENT_CLASS_TO_MANAGED_AGENT = { { "moe.yushi.authlibinjector.Premain",
                                                                                   "moe.yushi:authlibinjector" } };

struct Agent {
    /// The library pointing to the jar this Java agent is contained within
    LibraryPtr library;

    /// The argument to the Java agent, passed after an = if present
    QString argument;
};

# Zero-Copy Resource Resolution for Debug Builds

## Overview

This document describes a development-time mechanism designed to maximize iteration speed.

> ⚠️ **THIS SYSTEM IS ONLY ACTIVE IN DEBUG BUILDS**

In this mode, the executable:

- Accesses resources directly from the workspace
- Avoids any copy or staging step
- Resolves resources through project dependencies
- Works directly on source files (including with hot reload)
- Runs without any separation between edited assets and runtime assets

The executable effectively becomes a runtime entry point into the development workspace.

---

## Core Concept

In **DEBUG**, the executable is built with knowledge of:

- Its own resource root
- The resource roots of dependent projects

This information is injected at **build time**.

As a result, the runtime reads assets directly from their source location.

---

## Resource Access

In **DEBUG**:

- No resource copying occurs
- Assets are loaded from their original location in the workspace

This means:
- Editing a file immediately affects what the executable sees
- There is no intermediate “build version” of assets

---

## Iteration Workflow

### Traditional

1. Compile  
2. Copy resources  
3. Run  
4. Modify resource  
5. Copy again  
6. Restart  

---

### With this system

1. Compile  
2. Run  
3. Modify resource  
4. Restart or Hot Reload  

---

## Hot Reload

Because resources are read directly from source:

- Changes are immediately visible
- The runtime can reload data without restarting

This applies naturally to:
- Shaders
- Config files
- Textures
- Any file-based asset

---

## Benefits

- Faster iteration loop
- No resource duplication
- No desynchronization between source and runtime
- Direct use of versioned files
- Transparent multi-project resource access

---

## Debug vs Release

In **DEBUG**:
- Resources come from the workspace
- No copy or packaging is involved

In **RELEASE**:
- Resources are expected to be packaged or installed
- The executable no longer depends on the workspace

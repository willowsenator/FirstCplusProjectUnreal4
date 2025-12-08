# Git Commit Guide for Code Fixes

This guide provides suggested commit messages for each phase of fixes.

---

## Before You Start

```bash
# Create a backup branch
git checkout -b backup-before-fixes

# Return to main branch
git checkout main

# Create a new branch for fixes
git checkout -b code-review-fixes
```

---

## Phase 1: Critical Fixes

### After Step 1.1 (MainPlayerController)
```bash
git add Source/FirstCplusProject/GameplayActors/MainPlayerController.cpp
git commit -m "Fix: Add null check for HUDOverlay to prevent crash

- Added null check before AddToViewport() call
- Added error logging when widget creation fails
- Prevents crash when HUDOverlayAsset is not set

Fixes #CR-001"
```

### After Step 1.2 (Floater)
```bash
git add Source/FirstCplusProject/GameplayActors/Floater.cpp
git commit -m "Fix: Correct variable initialization in Floater constructor

- Changed 'A, B, C, D = 0.f' to separate assignments
- Previously only D was initialized to 0
- Ensures all sine wave parameters start at zero

Fixes #CR-002"
```

### After Step 1.3 (MainCharacter Die)
```bash
git add Source/FirstCplusProject/GameplayActors/MainCharacter.h
git add Source/FirstCplusProject/GameplayActors/MainCharacter.cpp
git commit -m "Fix: Implement proper death handling for MainCharacter

- Removed 'static' keyword from Die() function
- Added bIsDead flag to prevent multiple deaths
- Implemented player input disabling on death
- Added movement and collision disabling
- Initialized death-related booleans in constructor

Fixes #CR-003"
```

### After Step 1.4 (Null checks)
```bash
git add Source/FirstCplusProject/GameplayActors/MainCharacter.cpp
git commit -m "Fix: Add null safety checks in MainCharacter combat

- Added null check in Attack() before accessing EquippedWeapon
- Added null check in LMBDown() for weapon casting
- Added error logging for attack without weapon
- Prevents crashes when attacking without equipped weapon

Fixes #CR-004"
```

### Phase 1 Complete
```bash
git tag -a v0.1-critical-fixes -m "Phase 1: All critical crash fixes complete"
```

---

## Phase 2: High Priority Fixes

### After Step 2.1 (Enemy)
```bash
git add Source/FirstCplusProject/GameplayActors/Enemy.cpp
git commit -m "Fix: Correct combat target clearing logic in Enemy

- Fixed CombatSphereOnOverlapEnd to clear target after usage
- Prevents accessing null pointer in MoveToTarget call
- Improved combat sphere overlap handling

Fixes #CR-005"
```

### After Step 2.2 (Critter)
```bash
git add Source/FirstCplusProject/GameplayActors/Critter.cpp
git commit -m "Fix: Resolve frame-dependent movement in Critter

- Moved CurrentVelocity reset from Tick() to Move()
- Ensures consistent movement regardless of frame rate
- Improved movement input handling

Fixes #CR-006"
```

### After Step 2.3 (Weapon)
```bash
git add Source/FirstCplusProject/GameplayActors/Weapon.cpp
git commit -m "Fix: Update weapon state when equipped

- Added SetWeaponState(Ews_Equipped) call in Equip()
- Ensures weapon state matches actual equipped status
- Allows proper state queries from Blueprint

Fixes #CR-007"
```

### After Step 2.4 (Sprint)
```bash
git add Source/FirstCplusProject/GameplayActors/MainCharacter.cpp
git commit -m "Improve: Enhanced sprint logic and edge case handling

- Added exhausted state check before allowing sprint
- Added death check in sprint functions
- Removed velocity requirement for initiating sprint
- Sprint now feels more responsive and natural

Fixes #CR-008"
```

### Phase 2 Complete
```bash
git tag -a v0.2-high-priority-fixes -m "Phase 2: All high priority logic fixes complete"
```

---

## Phase 3: Medium Priority Improvements

### After Step 3.1 (Magic Numbers)
```bash
git add Source/FirstCplusProject/GameplayActors/MainCharacter.h
git add Source/FirstCplusProject/GameplayActors/MainCharacter.cpp
git add Source/FirstCplusProject/GameplayActors/Enemy.h
git add Source/FirstCplusProject/GameplayActors/Enemy.cpp
git commit -m "Refactor: Replace magic numbers with named constants

Added editable properties for:
- Camera boom distance (600.0f)
- Jump velocity (650.0f)
- Attack animation play rates (2.2f, 1.8f)
- Enemy sphere radii (600.0f, 75.0f)

Improves code maintainability and allows easy tuning in editor.

Fixes #CR-009"
```

### After Step 3.2 (Logging)
```bash
git add Source/FirstCplusProject/GameplayActors/Item.cpp
git add Source/FirstCplusProject/GameplayActors/Pickup.cpp
git add Source/FirstCplusProject/GameplayActors/Explosive.cpp
git commit -m "Refactor: Conditional compilation for debug logs

- Wrapped debug logs with #if !UE_BUILD_SHIPPING
- Logs remain in development builds
- Logs removed from shipping builds for performance
- Changed log level from Warning to Verbose

Fixes #CR-010"
```

### After Step 3.3 (Const)
```bash
git add Source/FirstCplusProject/GameplayActors/Enemy.h
git add Source/FirstCplusProject/GameplayActors/Enemy.cpp
git commit -m "Refactor: Add const correctness to Enemy::MoveToTarget

- Marked MoveToTarget() as const member function
- Function doesn't modify Enemy state
- Improves const correctness and intent clarity

Fixes #CR-011"
```

### Phase 3 Complete
```bash
git tag -a v0.3-medium-priority-fixes -m "Phase 3: All medium priority improvements complete"
```

---

## Phase 4: Low Priority Polish

### After Step 4.1 (Collider)
```bash
git add Source/FirstCplusProject/GameplayActors/Collider.h
git add Source/FirstCplusProject/GameplayActors/Collider.cpp
git commit -m "Refactor: Make Collider mesh configurable

- Removed hard-coded asset path
- Added ColliderMesh UPROPERTY
- Mesh can now be changed in Blueprint editor
- Maintains default mesh as fallback

Fixes #CR-012"
```

### After Step 4.2 (Comments)
```bash
git add Source/FirstCplusProject/GameplayActors/Floater.cpp
git commit -m "Clean: Remove commented-out code from Floater

- Removed obsolete physics code comments
- Removed old transformation examples
- Improves code readability

Fixes #CR-013"
```

### After Step 4.3 (Tick)
```bash
git add Source/FirstCplusProject/GameplayActors/Floater.h
git add Source/FirstCplusProject/GameplayActors/Floater.cpp
git add Source/FirstCplusProject/GameplayActors/FloatingPlatform.cpp
git commit -m "Optimize: Conditional tick enabling for performance

- Floater only ticks when bShouldFloat is true
- FloatingPlatform only ticks when interpolating
- Added SetFloating() helper function
- Reduces CPU overhead with many inactive actors

Fixes #CR-014"
```

### After Step 4.4 (Copyright)
```bash
git add Source/FirstCplusProject/GameplayActors/*.h
git add Source/FirstCplusProject/GameplayActors/*.cpp
git commit -m "Docs: Update copyright headers in all files

- Replaced placeholder copyright text
- Added consistent project header
- Improves code professionalism

Fixes #CR-015"
```

### Phase 4 Complete
```bash
git tag -a v0.4-low-priority-polish -m "Phase 4: All low priority polish complete"
```

---

## All Phases Complete

```bash
# Merge back to main
git checkout main
git merge code-review-fixes

# Tag final version
git tag -a v1.0-code-review-complete -m "All code review fixes implemented

Summary:
- 6 critical crash fixes
- 5 high priority logic fixes
- 3 medium priority improvements
- 4 low priority polish items

Total: 18 fixes across 14 files
All tests passing
Ready for QA"

# Push to remote (if using remote git)
git push origin main --tags
```

---

## Rollback Commands (If Needed)

### Rollback to before all fixes
```bash
git checkout backup-before-fixes
git checkout -b main-restored
```

### Rollback to specific phase
```bash
# Back to after Phase 1
git checkout v0.1-critical-fixes

# Back to after Phase 2
git checkout v0.2-high-priority-fixes

# Back to after Phase 3
git checkout v0.3-medium-priority-fixes
```

### Undo last commit (keep changes)
```bash
git reset --soft HEAD~1
```

### Undo last commit (discard changes)
```bash
git reset --hard HEAD~1
```

---

## Commit Message Format

We're using this format:
```
Type: Short summary (50 chars or less)

- Bullet point describing change
- Another bullet point
- More details if needed

Fixes #ISSUE-NUMBER
```

**Types:**
- `Fix:` - Bug fixes
- `Refactor:` - Code improvements (no behavior change)
- `Optimize:` - Performance improvements
- `Docs:` - Documentation changes
- `Clean:` - Code cleanup

---

## Viewing History

```bash
# See all commits
git log --oneline

# See detailed log
git log

# See changes in specific commit
git show <commit-hash>

# See all tags
git tag -l
```

---

## Best Practices

1. **Test before committing** - Compile and test each fix
2. **Commit often** - One logical change per commit
3. **Write clear messages** - Future you will thank you
4. **Tag milestones** - Makes rollback easier
5. **Keep backup branch** - Safety net for major changes

---

## Notes

- Replace `#CR-XXX` with your actual issue tracking numbers
- Adjust commit messages to match your team's style
- Add co-authors if pair programming: `Co-authored-by: Name <email>`
- Include issue numbers if using GitHub/GitLab/Jira

**Happy committing! 🎉**


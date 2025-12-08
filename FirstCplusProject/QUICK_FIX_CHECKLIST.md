# Quick Fix Checklist ✅

**Date:** 2025-12-08  
**Total Fixes:** 18 items across 4 phases

---

## 🔴 PHASE 1: Critical Fixes (DO FIRST!)

### ⚡ Crash Fixes
- [ ] **MainPlayerController.cpp** - Add null check before `HUDOverlay->AddToViewport()`
- [ ] **Floater.cpp** - Fix `A, B, C, D = 0.f;` → separate assignments
- [ ] **MainCharacter.h** - Remove `static` from `Die()`, add `bool bIsDead`
- [ ] **MainCharacter.cpp** - Implement proper `Die()` with input disable
- [ ] **MainCharacter.cpp** - Add null check in `Attack()` for `EquippedWeapon`
- [ ] **MainCharacter.cpp** - Add null check in `LMBDown()` for weapon cast

**✅ Test:** Game launches, no crashes on death/attack/HUD

---

## 🟡 PHASE 2: High Priority Logic Fixes

- [ ] **Enemy.cpp** - Fix `CombatSphereOnOverlapEnd()` - move `CombatTarget = nullptr` after usage
- [ ] **Critter.cpp** - Move `CurrentVelocity` reset from `Tick()` to `Move()`
- [ ] **Weapon.cpp** - Call `SetWeaponState(Ews_Equipped)` in `Equip()`
- [ ] **MainCharacter.cpp** - Fix `StartSprinting()` - check exhausted state, remove velocity check
- [ ] **MainCharacter.cpp** - Update `StopSprinting()` - check if dead

**✅ Test:** Combat works, movement smooth, sprint logic correct

---

## 🟠 PHASE 3: Medium Priority Improvements

- [ ] **MainCharacter.h** - Add properties: `CameraBoomDistance`, `CharacterJumpVelocity`, `Attack1/2PlayRate`
- [ ] **MainCharacter.cpp** - Replace magic numbers with new properties
- [ ] **Enemy.h** - Add `AgroSphereRadius`, `CombatSphereRadius` properties
- [ ] **Enemy.cpp** - Replace sphere radius magic numbers
- [ ] **Item.cpp** - Wrap logs with `#if !UE_BUILD_SHIPPING`
- [ ] **Pickup.cpp** - Wrap logs with `#if !UE_BUILD_SHIPPING`
- [ ] **Explosive.cpp** - Wrap logs with `#if !UE_BUILD_SHIPPING`
- [ ] **Enemy.h** - Mark `MoveToTarget()` as `const`

**✅ Test:** Values editable in editor, logs only in dev builds

---

## 🟢 PHASE 4: Low Priority Polish

- [ ] **Collider.h** - Add `UStaticMesh* ColliderMesh` property
- [ ] **Collider.cpp** - Make mesh configurable, set in `BeginPlay()`
- [ ] **Floater.cpp** - Remove commented code (lines 51-53, 58-83)
- [ ] **Floater.cpp** - Add `SetActorTickEnabled(bShouldFloat)` in `BeginPlay()`
- [ ] **FloatingPlatform.cpp** - Toggle tick with `bInterping`
- [ ] **All files** - Replace copyright comment with project name

**✅ Test:** Performance improved, code cleaner

---

## 📋 File Summary

| File | # of Changes | Priority |
|------|-------------|----------|
| MainCharacter.cpp | 7 | 🔴 Critical |
| MainCharacter.h | 4 | 🔴 Critical |
| Enemy.cpp | 3 | 🟡 High |
| Enemy.h | 2 | 🟠 Medium |
| MainPlayerController.cpp | 1 | 🔴 Critical |
| Floater.cpp | 3 | 🔴 Critical |
| Weapon.cpp | 1 | 🟡 High |
| Critter.cpp | 1 | 🟡 High |
| Item.cpp | 1 | 🟠 Medium |
| Pickup.cpp | 1 | 🟠 Medium |
| Explosive.cpp | 1 | 🟠 Medium |
| Collider.h | 1 | 🟢 Low |
| Collider.cpp | 1 | 🟢 Low |
| FloatingPlatform.cpp | 1 | 🟢 Low |

---

## ⏱️ Time Estimates

- **Phase 1 (Critical):** 1-2 hours ⚡ MUST DO
- **Phase 2 (High):** 1-2 hours
- **Phase 3 (Medium):** 1-2 hours
- **Phase 4 (Low):** 30-60 minutes

**Total:** 4-6 hours for complete implementation

---

## 🎯 Minimum Viable Fix

If time is limited, **ONLY do Phase 1** (Critical Fixes):
- Prevents crashes
- Makes game playable
- Safe to ship for testing

Everything else can wait!

---

## 🚀 Quick Start

1. **Backup your project**
2. **Start with Phase 1**
3. **Compile after each file**
4. **Test after each phase**
5. **Commit to git after each phase**

See `CODE_FIX_PLAN.md` for detailed implementation steps!

---

## 📞 Need Help?

If you get stuck on any fix, refer to:
- **CODE_FIX_PLAN.md** - Detailed step-by-step instructions
- **Code_Review_Report.md** - Original issue analysis
- Unreal Engine documentation

**You've got this! 💪**


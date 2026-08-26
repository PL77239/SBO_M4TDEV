#pragma once
#include <cstdint>
#include <ctime>
#include <unordered_map>

class Client;

// Base completion fare for Time Attack A/B.
// Tune these before shipping an economy pass.
#ifndef TA_BASE_CP
#define TA_BASE_CP 1500u
#endif
#ifndef TA_BASE_XP
#define TA_BASE_XP 200u
#endif
#ifndef TA_COOLDOWN_SEC
#define TA_COOLDOWN_SEC 120
#endif

// Call when the player leaves COURSE_TIMEATTACKA / B via 0x0302
// after having joined that course (base fare — no personal times required).
bool TimeAttack_TryGrantBaseFare(Client* client, uint32_t leavingCourse);

// Call later when the official TA finish-time packet is mapped.
// timeMs == 0 means "unknown / treat as base only".
bool TimeAttack_GrantFinishReward(Client* client, uint32_t courseId, uint32_t timeMs);

// Optional: mark that the client entered a TA course (0x0301 / join).
void TimeAttack_OnEnter(Client* client, uint32_t courseId);

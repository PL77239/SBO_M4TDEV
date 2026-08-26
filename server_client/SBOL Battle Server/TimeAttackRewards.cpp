#include "TimeAttackRewards.h"
#include "client.h"
#include "server.h"
#include <string>
#include <cstdio>

namespace
{
	struct TaSession
	{
		uint32_t courseId = 0;
		time_t enteredAt = 0;
		time_t lastPayoutAt = 0;
		bool entered = false;
	};

	std::unordered_map<uint32_t, TaSession>& sessions()
	{
		static std::unordered_map<uint32_t, TaSession> map;
		return map;
	}

	bool isTimeAttackCourse(uint32_t courseId)
	{
		return courseId == Client::COURSETYPE::COURSE_TIMEATTACKA
			|| courseId == Client::COURSETYPE::COURSE_TIMEATTACKB;
	}

	TaSession& sessionFor(Client* client)
	{
		return sessions()[client->driverslicense];
	}

	bool grant(Client* client, uint32_t cp, uint32_t xp, const wchar_t* reason)
	{
		if (!client || !client->server)
			return false;

		TaSession& s = sessionFor(client);
		time_t now = time(nullptr);
		if (s.lastPayoutAt != 0 && (now - s.lastPayoutAt) < TA_COOLDOWN_SEC)
		{
			if (client->logger)
			{
				client->logger->Log(Logger::LOGTYPE_CLIENT,
					L"Time Attack payout skipped for %s (%u): cooldown (%ld s left).",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					(long)(TA_COOLDOWN_SEC - (now - s.lastPayoutAt)));
			}
			return false;
		}

		client->giveCP((int64_t)cp);
		client->addExp(xp);
		client->server->saveClientData(client);
		s.lastPayoutAt = now;
		s.entered = false;

		if (client->logger)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT,
				L"Time Attack reward [%s] for %s (%u): +%u CP +%u XP (course %u).",
				reason,
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				cp,
				xp,
				s.courseId);
		}

		char msg[128];
		sprintf_s(msg, "TIME ATTACK: +%u CP / +%u XP", cp, xp);
		std::string announce(msg);
		client->SendAnnounceMessage(announce, RGB(80, 200, 120), client->driverslicense);
		return true;
	}
}

void TimeAttack_OnEnter(Client* client, uint32_t courseId)
{
	if (!client || !isTimeAttackCourse(courseId))
		return;
	TaSession& s = sessionFor(client);
	s.courseId = courseId;
	s.enteredAt = time(nullptr);
	s.entered = true;
}

bool TimeAttack_TryGrantBaseFare(Client* client, uint32_t leavingCourse)
{
	if (!client || !isTimeAttackCourse(leavingCourse))
		return false;

	TaSession& s = sessionFor(client);
	// If OnEnter was never wired, still allow payout when leaving a TA course
	// so a minimal 0x0302-only integration works.
	if (!s.entered)
	{
		s.courseId = leavingCourse;
		s.enteredAt = time(nullptr);
		s.entered = true;
	}

	return grant(client, TA_BASE_CP, TA_BASE_XP, L"base fare");
}

bool TimeAttack_GrantFinishReward(Client* client, uint32_t courseId, uint32_t timeMs)
{
	if (!client || !isTimeAttackCourse(courseId))
		return false;

	TaSession& s = sessionFor(client);
	s.courseId = courseId;

	uint32_t cp = TA_BASE_CP;
	uint32_t xp = TA_BASE_XP;

	// Placeholder tiers — replace with real threshold table once times exist.
	// timeMs == 0 → base only.
	if (timeMs != 0)
	{
		if (timeMs < 180000) { cp += 2000; xp += 300; }      // under 3:00
		else if (timeMs < 210000) { cp += 1000; xp += 150; } // under 3:30
		else if (timeMs < 240000) { cp += 500; xp += 75; }   // under 4:00
	}

	(void)timeMs;
	return grant(client, cp, xp, L"finish");
}

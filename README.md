This version currently offers multiplayer as a freeroam, but lacks a lot of features that would make it more vivid.

1) Battles; from my understanding, they are 100% server-sided, and should be activated using high beams (C) - yet nothing happens when approaching a rival. NPC Battles are half-baked in the code, i believe they lack AI driving;
When releasing safemode the server pings that packet.
High beams only change the last number in hex (logs); 00-01-00. The battle structure is there, but it doesn't work.
If neccessary, rebuild from scratch knowing the structure and relatives. 
2) Team Area / Team Center - Primarly meant to be used in a browser to register teams; the code is there, possibly cut - if possible, at one instance, create a simple (geocities style, 2005-http) webpage enabling users to register (so far done in cmd at DB; "SBOL DB Server.exe" /createaccount name pass email admin level(0-255)); and introduce teams (required data is on the DB tables). Make driving into Team Center / Team Area effective (so far it would take you to the webpage, but i have no idea what would happen afterwards. To test it first, create a team in DB with members known as users so far registered, or new ones. 
3) It's possible that the NPC Rivals are unable to be battled as they mismatch the rival count displayed in the DATA section when in-game. In theory, the number of NPC spawns only on client's side (invisible to others and vice-versa), or they're not in the DB therefore the server doesn't know where to send the packets to
Working EXE's for the server were pulled from /release. 

Long before the online worked, but since the server code had to be rebuild by anyone attempting it, it is being gatekept; 
https://www.youtube.com/watch?v=r1kup2Sf8PM

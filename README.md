**Notes:**

* Tested using a listen server and up to 3 clients
* To change the threshold at which rollback is disabled, check BP_TankCharacter -> RollbackComponent -> MaxHistoryTime
* Client-side navigation is used for pathfinding

---

**Console Commands:**
* rollback.DrawFrameHistory true -> Use this to see the frame history of all characters on the server.
* rollback.DrawHitCapsule true -> Use this to see the server rollback collision result. Green means the collision test was successful and the client was awarded a hit, red means it wasn’t successful.
* NetEmulation.PktLag [NUM] -> For example, calling NetEmulation.PktLag 100 on a client will set that client's ping to around 100. If you set it to more than 250(MaxHistoryTime), server rollback will be disabled to prevent unfair advantages for players with high ping.

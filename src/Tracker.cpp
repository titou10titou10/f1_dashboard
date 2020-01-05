#include <QDateTime>

#include "Tracker.h"

Tracker::Tracker(){}

void Tracker::headerData(const PacketHeader &header) {

   if (sessionUID != header.m_sessionUID){
      timeLastLaps.clear();
      timeSector1.clear();
      timeSector2.clear();

      timeLastLaps.squeeze();
      timeSector1.squeeze();
      timeSector2.squeeze();

      sessionUID = header.m_sessionUID;
      qDebug() << ">>>> New session:" <<  sessionUID;
   }
}

void Tracker::telemetryData(const PacketHeader &header, const PacketCarTelemetryData &data) {
    auto ixPlayer = header.m_playerCarIndex;
    emit(telemetryChanged(header, data.m_carTelemetryData[ixPlayer]));
}

void Tracker::lapData(const PacketHeader &header, const PacketLapData &data) {
    auto ixPlayer = header.m_playerCarIndex;
    auto playerData = data.m_lapData[ixPlayer];

    QString namePrev = "-";
    int ixPrev = -1;
    float deltaPrev = 0;
    QString nameFollow = "-";
    int ixFollow = -1;
    float deltaFollow = 0;

    // Only compute delta times for races or Qualif sessions
    if ((sessionType == 0) || (sessionType == 12)) {
       emit(lapChanged(header, data.m_lapData[ixPlayer], namePrev, deltaPrev, nameFollow, deltaFollow));
       return;
    }

    // Find index and name of prev car
    int posPrev = playerData.m_carPosition - 1;
    if (posPrev >= 1) {
       for(int i = 0; i < numActiveCars; i++) {
          if (data.m_lapData[i].m_carPosition == posPrev) {
             ixPrev = i;
             namePrev = participants[i].m_name;
             break;
          }
       }
    }

    // Find index and name of following car
    int posFollow = playerData.m_carPosition + 1;
    if (posFollow <= numActiveCars) {
        for(int i = 0; i < numActiveCars; i++) {
           if (data.m_lapData[i].m_carPosition == posFollow) {
              ixFollow = i;
              nameFollow = participants[i].m_name;
              break;
           }
        }
    }

    // --------------------------------------
    // For qualif session, compare best times
    // --------------------------------------

    if ((sessionType != 10) && (sessionType != 11)) {

       float bestLapTimePlayer = data.m_lapData[ixPlayer].m_bestLapTime;
       if (bestLapTimePlayer > 0) {
          if (ixPrev != -1) {
             if (data.m_lapData[ixPrev].m_bestLapTime > 0) {
                deltaPrev = data.m_lapData[ixPrev].m_bestLapTime - bestLapTimePlayer;
             }
          }
          if (ixFollow != -1) {
             if (data.m_lapData[ixFollow].m_bestLapTime > 0) {
                deltaPrev = data.m_lapData[ixFollow].m_bestLapTime - bestLapTimePlayer;
             }
          }
       }
       emit(lapChanged(header, data.m_lapData[ixPlayer], namePrev, deltaPrev, nameFollow, deltaFollow));
       return;
    }

    // --------------------------------------
    // For Races, compare total time
    // --------------------------------------

    // Store current lap time per participant, per lap
    if (timeLastLaps.empty()){
        timeLastLaps.fill(QVector<float>(), numActiveCars);
    }
    if (timeSector1.empty()){
        timeSector1.fill(QVector<float>(), numActiveCars);
    }
    if (timeSector2.empty()){
        timeSector2.fill(QVector<float>(), numActiveCars);
    }


    for(int i = 0; i < numActiveCars; i++) {
      auto pData = data.m_lapData[i];
      auto lapNum = pData.m_currentLapNum;

      int index = lapNum - 1;

      // Store sector 1 times
      if (timeSector1[i].size() <= index) {
         timeSector1[i].append(pData.m_sector1Time);
      } else {
         timeSector1[i][index] = pData.m_sector1Time;
      }

      // Store sector /2 times
      if (timeSector2[i].size() <= index) {
         timeSector2[i].append(pData.m_sector2Time);
      } else {
         timeSector2[i][index] = pData.m_sector2Time;
      }

      // Store last lap time
      if (lapNum > 1) {
         int indexLL = lapNum - 2;
         if (timeLastLaps[i].size() <= indexLL) {
             timeLastLaps[i].append(pData.m_lastLapTime);
          } else {
             timeLastLaps[i][indexLL] = pData.m_lastLapTime;
          }
      }
    }

    // Compute total time up to last sector for the lap/sector the player is...

    auto playerLapIndex = playerData.m_currentLapNum - 1;
    float s1 = timeSector1[ixPlayer][playerLapIndex];
    float s2 = timeSector2[ixPlayer][playerLapIndex];
    bool addSector1 = (s1 > 0);
    bool addSector2 = (s2 > 0);

//    qDebug() << playerIndex <<  "apres PL" << timeSector1[playerIndex].at(0) << playerLapIndex;
//    qDebug() << ixPrev <<  "apres P" << timeSector1[ixPrev].at(0);

    float playerTotal = s1 + s2;
    for(int j = 0; j < playerLapIndex; j++) {
       playerTotal += timeLastLaps[ixPlayer][j];
    }

    float prevTotal = 0;
    if (ixPrev != -1) {
       if (addSector1) prevTotal += timeSector1[ixPrev].value(playerLapIndex);
       if (addSector2) prevTotal += timeSector2[ixPrev].value(playerLapIndex);
       for(int j = 0; j < playerLapIndex; j++) {
          prevTotal += timeLastLaps[ixPrev].value(j);
       }
    }

    float followTotal = 0;
    if (ixFollow != -1) {
       if (addSector1) followTotal += timeSector1[ixFollow].value(playerLapIndex);
       if (addSector2) followTotal += timeSector2[ixFollow].value(playerLapIndex);
       for(int j = 0; j < playerLapIndex; j++) {
          followTotal += timeLastLaps[ixFollow].value(j);
       }
    }

    // Compute delta time with prev and follow


     if (playerData.m_carPosition > 1) {
        deltaPrev = prevTotal - playerTotal;
     }
     if (playerData.m_carPosition < numActiveCars) {
        deltaFollow = followTotal - playerTotal;
     }

     // qDebug() << "Prev" << namePrev << deltaPrev << "Follow" << nameFollow << deltaFollow;

     emit(lapChanged(header, data.m_lapData[ixPlayer], namePrev, deltaPrev, nameFollow, deltaFollow));
}

void Tracker::sessionData(const PacketHeader &header, const PacketSessionData &data){
    sessionType = data.m_sessionType;
    emit(sessionChanged(header, data));
}

void Tracker::setupData(const PacketHeader &header, const PacketCarSetupData &data) {
    auto ixPlayer = header.m_playerCarIndex;
    emit(setupChanged(header, data.m_carSetups[ixPlayer]));
}

void Tracker::statusData(const PacketHeader &header, const PacketCarStatusData &data) {
    auto ixPlayer = header.m_playerCarIndex;
    emit(statusChanged(header, data.m_carStatusData[ixPlayer]));
}

void Tracker::participantData(const PacketHeader &header, const PacketParticipantsData &data) {
    numActiveCars = data.m_numActiveCars;
    participants = data.m_participants;

    auto ixPlayer = header.m_playerCarIndex;
    emit(participantChanged(header, data.m_participants[ixPlayer]));
}

void Tracker::motionData(const PacketHeader &header, const PacketMotionData &data) {
    Q_UNUSED(header);
    Q_UNUSED(data);

//    auto playerIndex = header.m_playerCarIndex;
//    auto playerData = data.m_carMotionData.at(playerIndex);
//    emit(motionChanged(header, playerData));
}

void Tracker::eventData(const PacketHeader &header, const PacketEventData &data) {
    emit(eventChanged(header, data));
}

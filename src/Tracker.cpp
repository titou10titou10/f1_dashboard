#include <QDateTime>

#include "Tracker.h"

Tracker::Tracker(){}

void Tracker::telemetryData(const PacketHeader &header, const PacketCarTelemetryData &data) {
    auto playerIndex = header.m_playerCarIndex;
    emit(telemetryChanged(header, data.m_carTelemetryData.at(playerIndex)));
}

void Tracker::lapData(const PacketHeader &header, const PacketLapData &data) {
    auto playerIndex = header.m_playerCarIndex;
    emit(lapChanged(header, data.m_lapData.at(playerIndex)));
}

void Tracker::sessionData(const PacketHeader &header, const PacketSessionData &data){
    emit(sessionChanged(header, data));
}

void Tracker::setupData(const PacketHeader &header, const PacketCarSetupData &data) {
    auto playerIndex = header.m_playerCarIndex;
    emit(setupChanged(header, data.m_carSetups.at(playerIndex)));
}

void Tracker::statusData(const PacketHeader &header, const PacketCarStatusData &data) {
    auto playerIndex = header.m_playerCarIndex;
    emit(statusChanged(header, data.m_carStatusData.at(playerIndex)));
}

void Tracker::participantData(const PacketHeader &header, const PacketParticipantsData &data) {
    auto playerIndex = header.m_playerCarIndex;
    emit(participantChanged(header, data.m_participants.at(playerIndex)));
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

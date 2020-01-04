#ifndef TRACKER_H
#define TRACKER_H

#include <QDir>
#include <QVector>

#include "UdpListener.h"
#include "ui_F1Status.h"

class Tracker : public QObject, public F1PacketInterface {
    Q_OBJECT

  public:
    Tracker();
    virtual ~Tracker() override {}

  signals:
    void telemetryChanged(const PacketHeader &header, const CarTelemetryData &data) ;
    void lapChanged(const PacketHeader &header, const LapData &data, const QString namePrev,float deltaPrev, QString nameFollow, float deltaFollow);
    void sessionChanged(const PacketHeader &header, const PacketSessionData &data);
    void setupChanged(const PacketHeader &header, const CarSetupData &data);
    void statusChanged(const PacketHeader &header, const CarStatusData &data);
    void participantChanged(const PacketHeader &header, const ParticipantData &data);
    void motionChanged(const PacketHeader &header, const CarMotionData &data);
    void eventChanged(const PacketHeader &header, const PacketEventData &data);

  private:
    // F1PacketInterface interface
    void headerData(const PacketHeader &header) override;
    void telemetryData(const PacketHeader &header, const PacketCarTelemetryData &data) override;
    void lapData(const PacketHeader &header, const PacketLapData &data) override;
    void sessionData(const PacketHeader &header, const PacketSessionData &data) override;
    void setupData(const PacketHeader &header, const PacketCarSetupData &data) override;
    void statusData(const PacketHeader &header, const PacketCarStatusData &data) override;
    void participantData(const PacketHeader &header, const PacketParticipantsData &data) override;
    void motionData(const PacketHeader &header, const PacketMotionData &data) override;
    void eventData(const PacketHeader &header, const PacketEventData &data) override;

    quint64 sessionUID;
    quint8  numActiveCars = 0;
    QVector<ParticipantData> participants;

    // Participant, Lap
    QVector<QVector<float>> timeLastLaps;
    QVector<QVector<float>> timeSector1;
    QVector<QVector<float>> timeSector2;

};

#endif // TRACKER_H

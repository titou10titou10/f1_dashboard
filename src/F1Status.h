#ifndef F1STATUS_H
#define F1STATUS_H

#include <QMainWindow>
#include <QLabel>

#include "UdpListener.h"

QT_BEGIN_NAMESPACE
namespace Ui { class F1Status; }
QT_END_NAMESPACE

class Tracker;

class F1Status : public QMainWindow {
    Q_OBJECT

public:
    F1Status(QWidget *parent = nullptr);
    ~F1Status();

private:
    Ui::F1Status *ui;
    F1Listener *_listener;
    Tracker *_tracker;

    const char* fmtTwo       = "%02.2f";
    const char* fmtTwoSign   = "%-02.2f";
    const char* fmtThreeSign = "%+02.3f";
    const char* fmtPSI       = "%02.1f psi";
    const char* fmtPenalty   = "%+02.0f secs";

    const QString fmtMSSZZZ = "m:ss.zzz";
    const QString fmtHMMSS  = "h:mm:ss";
    const QString fmtMMSS   = "mm:ss";

    const QString colorPct    = "background-color: hsv(%1, 255, 180); color: rgb(0, 0, 0);";
    const QString colorRED    = "background-color: hsv(0, 255, 180); color: rgb(0, 0, 0);";
    const QString colorGREEN  = "background-color: hsv(120, 255, 180); color: rgb(0, 0, 0);";
    const QString colorORANGE = "background-color: hsv(60, 255, 180); color: rgb(0, 0, 0);";
    const QString colorGREY   = "background-color: rgb(192, 192, 192); color: rgb(0, 0, 0);";

    uint8_t drsAllowed = 0;

    void setColoredLabel(QLabel* label, const quint8 value);
    void setColor(QLabel* label, const QString fmtColor);

    void setTimeMs(QLabel* label, const float value);
    void setTimeS(QLabel* label, const uint16_t value);
    void setTimeDelta(QLabel* label, const float last, const float best);
    void setFloatFormat(QLabel* label, const char* fmt, const float value);

private slots:
    void telemetryChanged(const PacketHeader &header, const CarTelemetryData &data) ;
    void lapChanged(const PacketHeader &header, const LapData &data, const QString namePrev,float deltaPrev, QString nameFollow, float deltaFollow);
    void sessionChanged(const PacketHeader &header, const PacketSessionData &data);
    void setupChanged(const PacketHeader &header, const CarSetupData &data);
    void statusChanged(const PacketHeader &header, const CarStatusData &data);
    void participantChanged(const PacketHeader &header, const ParticipantData &data);
    void motionChanged(const PacketHeader &header, const CarMotionData &data);
    void eventChanged(const PacketHeader &header, const PacketEventData &data);

};
#endif // F1STATUS_H

#include "F1Status.h"
#include "UdpListener.h"
#include "Tracker.h"
#include "ui_F1Status.h"
#include <QThread>
#include <QDateTime>

F1Status::F1Status(QWidget *parent):QMainWindow(parent), ui(new Ui::F1Status) {

    qDebug() << "F1Status starting...";

    ui->setupUi(this);
    // setWindowIcon(QIcon(":/Ressources/F1Telemetry.png"));
    setWindowTitle(qApp->applicationName() + " " + qApp->applicationVersion());
    adjustSize();
//    resize(1400, 800);

    // Register structures passed from the UDP thread to this thread (GUI)
    qRegisterMetaType<PacketHeader>("PacketHeader");
    qRegisterMetaType<CarTelemetryData>("CarTelemetryData");
    qRegisterMetaType<LapData>("LapData");
    qRegisterMetaType<PacketSessionData>("PacketSessionData");
    qRegisterMetaType<CarSetupData>("CarSetupData");
    qRegisterMetaType<CarStatusData>("CarStatusData");
    qRegisterMetaType<ParticipantData>("ParticipantData");
    qRegisterMetaType<CarMotionData>("CarMotionData");
    qRegisterMetaType<PacketEventData>("PacketEventData");

    // Connect signals from the UDP thread
    _tracker = new Tracker();
    connect(_tracker, &Tracker::telemetryChanged, this, &F1Status::telemetryChanged);
    connect(_tracker, &Tracker::lapChanged, this, &F1Status::lapChanged);
    connect(_tracker, &Tracker::sessionChanged, this, &F1Status::sessionChanged);
    connect(_tracker, &Tracker::setupChanged, this, &F1Status::setupChanged);
    connect(_tracker, &Tracker::statusChanged, this, &F1Status::statusChanged);
    connect(_tracker, &Tracker::participantChanged, this, &F1Status::participantChanged);
    connect(_tracker, &Tracker::motionChanged, this, &F1Status::motionChanged);
    connect(_tracker, &Tracker::eventChanged, this, &F1Status::eventChanged);

    // Start UDP listener in a different thread that the current QT GUI event loop...
    QThread *thread = new QThread;
    F1Listener *f1_listener = new F1Listener(_tracker);
    f1_listener->moveToThread(thread);
    thread->start();
}

F1Status::~F1Status() { delete ui; }

// ------------------------
// Update GUI
// ------------------------

void F1Status::telemetryChanged(const PacketHeader &header, const CarTelemetryData &data) {
   Q_UNUSED(header);

   ui->car_speed->setNum(data.m_speed);
   ui->car_gear->setNum(data.m_gear);
   ui->engine_rpm->setNum(data.m_engineRPM);
   if (data.m_drs) {
       ui->car_drs->setChecked(true);
   } else {
       ui->car_drs->setChecked(false);
   }

   ui->temp_engine->setNum(data.m_engineTemperature);
   ui->temp_tyre_rl->setNum(data.m_tyresSurfaceTemperature[0]);
   ui->temp_tyre_rr->setNum(data.m_tyresSurfaceTemperature[1]);
   ui->temp_tyre_fl->setNum(data.m_tyresSurfaceTemperature[2]);
   ui->temp_tyre_fr->setNum(data.m_tyresSurfaceTemperature[3]);
   ui->temp_brake_rl->setNum(data.m_brakesTemperature[0]);
   ui->temp_brake_rr->setNum(data.m_brakesTemperature[1]);
   ui->temp_brake_fl->setNum(data.m_brakesTemperature[2]);
   ui->temp_brake_fr->setNum(data.m_brakesTemperature[3]);
   ui->pressure_rl->setText(setPSI(data.m_tyresPressure[0]));
   ui->pressure_rr->setText(setPSI(data.m_tyresPressure[1]));
   ui->pressure_fl->setText(setPSI(data.m_tyresPressure[2]));
   ui->pressure_fr->setText(setPSI(data.m_tyresPressure[3]));
}

void F1Status::lapChanged(const PacketHeader &header, const LapData &data){
    Q_UNUSED(header);

    ui->lap_penalties->setNum(data.m_penalties);
    ui->lap_position->setNum(data.m_carPosition);

    ui->lap_num->setNum(data.m_currentLapNum);

    ui->lap_time_best->setText(formatTimeMs(data.m_bestLapTime));
    ui->lap_time_last->setText(formatTimeMs(data.m_lastLapTime));
    ui->lap_time_current->setText(formatTimeMs(data.m_currentLapTime));
}

void F1Status::sessionChanged(const PacketHeader &header, const PacketSessionData &data){
   Q_UNUSED(header);

   ui->session_type->setText(UdpSpecification::instance()->session_type(data.m_sessionType));
   ui->session_track->setText(UdpSpecification::instance()->track(data.m_trackId));
   ui->session_time_left->setText(formatTimeS(data.m_sessionTimeLeft));
   ui->weather->setText(UdpSpecification::instance()->weather(data.m_weather));
   ui->temp_track->setNum(data.m_trackTemperature);
   ui->temp_air->setNum(data.m_airTemperature);

   if ((data.m_totalLaps > 0) && (data.m_totalLaps < 99)) {
      QString d = QString("/ %1").arg(data.m_totalLaps);
      ui->lap_total->setText(d);
   } else {
      ui->lap_total->clear();
   }

}

void F1Status::setupChanged(const PacketHeader &header, const CarSetupData &data){
    Q_UNUSED(header);
    Q_UNUSED(data);
}

void F1Status::statusChanged(const PacketHeader &header,const CarStatusData &data) {
   Q_UNUSED(header);

   ui->tyre_compound->setText(UdpSpecification::instance()->tyre(data.m_tyreCompound));
   ui->tyre_visual->setText(UdpSpecification::instance()->visualTyre(data.m_tyreVisualCompound));


   // ERS
   float h = (data.m_ersHarvestedThisLapMGUK + data.m_ersHarvestedThisLapMGUH) / 1000.0;
   float s = data.m_ersStoreEnergy / 1000.0;
   float d = data.m_ersDeployedThisLap / 1000.0;

   ui->ers_mode->setText(UdpSpecification::instance()->ersMode(data.m_ersDeployMode));
   ui->ers_harvested->setValue(h);
   ui->ers_storeenergy->setValue(s);
   ui->ers_deployed->setValue(d);

   // Fuel
   ui->fuel_mix->setText(UdpSpecification::instance()->fuelMix(data.m_fuelMix));
   ui->fuel_in_tank->setText(truncate2(data.m_fuelInTank));

   ui->car_fuel_laps->setText(truncate2plus(data.m_fuelRemainingLaps));
   if (data.m_fuelRemainingLaps >= 0) {
       setGreen(ui->car_fuel_laps);
   } else {
       setRed(ui->car_fuel_laps);
   }

   setColoredLabel(ui->dmg_tyre_wear_rl, data.m_tyresWear[0]);
   setColoredLabel(ui->dmg_tyre_wear_rr, data.m_tyresWear[1]);
   setColoredLabel(ui->dmg_tyre_wear_fl, data.m_tyresWear[2]);
   setColoredLabel(ui->dmg_tyre_wear_fr, data.m_tyresWear[3]);

   setColoredLabel(ui->dmg_engine, data.m_engineDamage);
   setColoredLabel(ui->dmg_gearbox, data.m_gearBoxDamage);

   setColoredLabel(ui->dmg_wing_fl, data.m_frontLeftWingDamage);
   setColoredLabel(ui->dmg_wing_fr, data.m_frontRightWingDamage);
   setColoredLabel(ui->dmg_wing_rear, data.m_rearWingDamage);
}

void F1Status::participantChanged(const PacketHeader &header, const ParticipantData &data){
   Q_UNUSED(header);

   ui->participant_name->setText(data.m_name);
}

void F1Status::motionChanged(const PacketHeader &header, const CarMotionData &data){
    Q_UNUSED(header);
    Q_UNUSED(data);
}

void F1Status::eventChanged(const PacketHeader &header, const PacketEventData &data){
    Q_UNUSED(header);
    Q_UNUSED(data);
}

// ------------------------
// Helper
// ------------------------
void F1Status::setColoredLabel(QLabel* item, quint8 value)  {
    auto oldValue = item->text();
    auto newValue = QString::number(value);
    if (oldValue.compare(newValue) != 0) {
       int hue = 120 - (value * 12 / 10);
       QString bc = QString("background-color: hsv(%1, 255, 180); color: rgb(0, 0, 0);").arg(hue);
       item->setNum(value);
       item->setStyleSheet(bc);
    }
}

void F1Status::setRed(QLabel* label){
   label->setStyleSheet("background-color: hsv(0, 255, 180); color: rgb(0, 0, 0);");
}

void F1Status::setGreen(QLabel* label){
   label->setStyleSheet("background-color: hsv(120, 255, 180);color: rgb(0, 0, 0);");
}

void F1Status::setOrange(QLabel* label){
   label->setStyleSheet("background-color: hsv(60, 255, 180)color: rgb(0, 0, 0);");
}

QString F1Status::formatTimeMs(float value)  {
   return QTime::fromMSecsSinceStartOfDay(value * 1000).toString("m:ss.zzz");
}

QString F1Status::formatTimeS(uint16_t value)  {
    if (value > 60 *60) {
       return QTime::fromMSecsSinceStartOfDay(value * 1000).toString("h:mm:ss");
    } else {
        return QTime::fromMSecsSinceStartOfDay(value * 1000).toString("mm:ss");
    }
}

QString F1Status::truncate2(float value)  {
    QString x;
    x.sprintf("%02.2f", value);
    return x;
}

QString F1Status::truncate2plus(float value)  {
    QString x;
    x.sprintf("%+02.2f", value);
    return x;
}

QString F1Status::setPSI(float value)  {
    return QString("%1 psi").arg(value);
}

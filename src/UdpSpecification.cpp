#include "UdpSpecification.h"

#include <QtDebug>


template <typename T> void readDataList(QDataStream &in, QVector<T> &list, int nb = 20) {
    list.reserve(nb);
    for(auto i = 0; i < nb; ++i) {
        T data;
        in >> data;
        list.append(data);
    }
}

int UdpSpecification::expectedPacketLength(UdpSpecification::PacketType type) const {
	return packetExpectedLengths[type];
}

UdpSpecification::UdpSpecification() {
	packetExpectedLengths[PacketType::Header] = 23;
	packetExpectedLengths[PacketType::Motion] = 1320;
	packetExpectedLengths[PacketType::Session] = 126;
	packetExpectedLengths[PacketType::LapData] = 820;
	packetExpectedLengths[PacketType::Event] = 9;
	packetExpectedLengths[PacketType::Participants] = 1081;
	packetExpectedLengths[PacketType::CarSetup] = 820;
	packetExpectedLengths[PacketType::CarTelemetry] = 1324;
	packetExpectedLengths[PacketType::CarStatus] = 1120;

	teams = QStringList({"Mercedes",
						 "Ferrari",
						 "Red Bull",
						 "Williams",
						 "Racing Point",
						 "Renault",
						 "Toro Rosso",
						 "Haas",
						 "McLaren",
						 "Alfa Romeo", // 0-9
						 "McLaren 1988",
						 "McLaren 1991",
						 "Williams 1992",
						 "Ferrari 1995",
						 "Williams 1996",
						 "McLaren 1998",
						 "Ferrari 2002",
						 "Ferrari 2004", // 10-17
						 "Renault 2006",
						 "Ferrari 2007",
						 "McLaren 2008 (deleted)",
						 "Red Bull 2010",
						 "Ferrari 1976", // 18-22
						 "ART Grand Prix",
						 "Campos Vexatec Racing",
						 "Carlin",
						 "Charouz Racing System",
						 "DAMS",
						 "Russian Time",
						 "MP Motorsport",
						 "Pertamina" // 23-30
						 "Mclaren 1990",
						 "Trident",
						 "BWT Arden",
						 "McLaren 1976",
						 "Lotus 1972",
						 "Ferrari 1979", // 31-36
						 "McLaren 1982",
						 "Williams 2003",
						 "Brawn 2009",
						 "Lotus 1978", // 37-40
						 "",
						 "Art GP",
						 "Campos",
						 "Carlin",
						 "Sauber Junior Charouz",
						 "Dams",
						 "Uni-Virtuosi",
						 "MP Motorsport",
						 "Prema",
						 "Trident",
						 "Arden",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "",
						 "", // 41-62
						 "Ferrari 1980",
						 "Mclaren 2010",
						 "Ferrari 2010"}); // 63-65
	tracks =
	QStringList({"Melbourne",   "Paul Ricard", "Shanghai",   "Sakir (Bahrain)", "Catalunya",         "Monaco",
				 "Montreal",    "Silverstone", "Hockenheim", "Hungaroring",     "Spa Francorchamp",  "Monza",
				 "Singapore",   "Suzuka",      "Abu Dhabi",  "Austin",          "Interlagos",        "Red Bull Ring",
				 "Sochi",       "Mexico",      "Baku",       "Sakhir Short",    "Silverstone Short", "Austin Short",
				 "Susuka Short"});
	raceLaps = {58, 53, 56, 57, 66, 78, 70, 52, 67, 70, 44, 53, 61, 53, 55, 56, 71, 71, 53, 71, 51, 0, 0, 0, 0};
	weathers = QStringList({"Clear", "Light Cloud", "Overcast", "Light Rain", "Heavy Rain", "Storm"});
	sessions =
	QStringList({"Unknown", "FP1", "FP2", "FP3", "Short FP", "Q1", "Q2", "Q3", "Short Q", "OSQ", "R1", "R2", "Time Trial"});
	tyres = QStringList({"Hyper Soft",
						 "Ultra Soft",
						 "Super Soft",
						 "Soft",
						 "Medium",
						 "Hard",
						 "Super Hard",
						 "Inter",
						 "Full Wet",
						 "Dry (Classic)",
						 "Wet (Classic)",
						 "Super Soft (F2)",
						 "Soft (F2)",
						 "Medium (F2)",
						 "Hard (F2)",
						 "Wet (F2)",
						 "C5",
						 "C4",
						 "C3",
						 "C2",
						 "C1"});
	visualTyres = QStringList({"Hyper Soft", "Ultra Soft", "Super Soft", "Soft", "Medium", "Hard", "Super Hard",
							   "Inter", "Full Wet", "Dry (Classic)", "Wet (Classic)", "Super Soft (F2)", "Soft (F2)",
							   "Medium (F2)", "Hard (F2)", "Wet (F2)", "Soft", "Medium", "Hard"});
	ersModes = QStringList({"None", "Low", "Medium", "High", "Overtake", "Hotlap"});
	fuelMixes = QStringList({"Lean", "Standard", "Rich", "Max"});
	formulaTypes = QStringList({"F1", "F1 Classic", "F2", "F1 Generic"});
	surfaces = QStringList({"Tarmac", "Rumble strip", "Concrete", "Rock", "Gravel", "Mud", "Sand", "Grass", "Water",
							"Cobblestone", "Metal", "Rigged"});
}

QDataStream &operator>>(QDataStream &in, PacketHeader &packet) {
	in >> packet.m_packetFormat >> packet.m_gameMajorVersion >> packet.m_gameMinorVersion >> packet.m_packetVersion >>
	packet.m_packetId >> packet.m_sessionUID >> packet.m_sessionTime >> packet.m_frameIdentifier >> packet.m_playerCarIndex;

	return in;
}

QDataStream &operator>>(QDataStream &in, ParticipantData &packet) {
	in >> packet.m_aiControlled >> packet.m_driverId >> packet.m_teamId >> packet.m_raceNumber >> packet.m_nationality;
	packet.m_name.clear();
	char name[48];
	for(auto i = 0; i < 48; ++i) {
		qint8 c;
		in >> c;
		name[i] = c;
	}

	packet.m_name = QString::fromUtf8(name);
	in >> packet.m_yourTelemetry;

	return in;
}

QDataStream &operator<<(QDataStream &out, const ParticipantData &packet) {
	out << packet.m_aiControlled << packet.m_driverId << packet.m_teamId << packet.m_raceNumber << packet.m_nationality;
	auto codedName = packet.m_name.toUtf8();
	for(auto i = 0; i < 48; ++i) {
		if(i < codedName.count()) {
			out << quint8(codedName[i]);
		} else {
			out << quint8(0);
		}
	}

	out << packet.m_yourTelemetry;

	return out;
}

QDataStream &operator>>(QDataStream &in, PacketParticipantsData &packet) {
	in >> packet.m_numActiveCars;
	readDataList<ParticipantData>(in, packet.m_participants);
	return in;
}

QDataStream &operator>>(QDataStream &in, LapData &packet) {
	in >> packet.m_lastLapTime >> packet.m_currentLapTime >> packet.m_bestLapTime >> packet.m_sector1Time >>
	packet.m_sector2Time >> packet.m_lapDistance >> packet.m_totalDistance >> packet.m_safetyCarDelta >>
	packet.m_carPosition >> packet.m_currentLapNum >> packet.m_pitStatus >> packet.m_sector >> packet.m_currentLapInvalid >>
	packet.m_penalties >> packet.m_gridPosition >> packet.m_driverStatus >> packet.m_resultStatus;

	return in;
}

QDataStream &operator>>(QDataStream &in, PacketLapData &packet) {
	readDataList<LapData>(in, packet.m_lapData);
	return in;
}

QDataStream &operator>>(QDataStream &in, CarTelemetryData &packet) {
	in >> packet.m_speed >> packet.m_throttle >> packet.m_steer >> packet.m_brake >> packet.m_clutch >> packet.m_gear >>
	packet.m_engineRPM >> packet.m_drs >> packet.m_revLightsPercent;
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_brakesTemperature[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_tyresSurfaceTemperature[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_tyresInnerTemperature[i];
	in >> packet.m_engineTemperature;
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_tyresPressure[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_surfaceType[i];

	return in;
}

QDataStream &operator>>(QDataStream &in, PacketCarTelemetryData &packet) {
	readDataList<CarTelemetryData>(in, packet.m_carTelemetryData);
	in >> packet.m_buttonStatus;
	return in;
}

QDataStream &operator>>(QDataStream &in, CarSetupData &packet) {
	in >> packet.m_frontWing >> packet.m_rearWing >> packet.m_onThrottle >> packet.m_offThrottle >>
	packet.m_frontCamber >> packet.m_rearCamber >> packet.m_frontToe >> packet.m_rearToe >> packet.m_frontSuspension >>
	packet.m_rearSuspension >> packet.m_frontAntiRollBar >> packet.m_rearAntiRollBar >>
	packet.m_frontSuspensionHeight >> packet.m_rearSuspensionHeight >> packet.m_brakePressure >> packet.m_brakeBias >>
	packet.m_frontTyrePressure >> packet.m_rearTyrePressure >> packet.m_ballast >> packet.m_fuelLoad;
	return in;
}

QDataStream &operator<<(QDataStream &out, const CarSetupData &packet) {
	out << packet.m_frontWing << packet.m_rearWing << packet.m_onThrottle << packet.m_offThrottle
		<< packet.m_frontCamber << packet.m_rearCamber << packet.m_frontToe << packet.m_rearToe
		<< packet.m_frontSuspension << packet.m_rearSuspension << packet.m_frontAntiRollBar << packet.m_rearAntiRollBar
		<< packet.m_frontSuspensionHeight << packet.m_rearSuspensionHeight << packet.m_brakePressure << packet.m_brakeBias
		<< packet.m_frontTyrePressure << packet.m_rearTyrePressure << packet.m_ballast << packet.m_fuelLoad;
	return out;
}

QDataStream &operator>>(QDataStream &in, PacketCarSetupData &packet) {
	readDataList<CarSetupData>(in, packet.m_carSetups);
	return in;
}


QDataStream &operator>>(QDataStream &in, MarshalZone &packet) {
	in >> packet.m_zoneStart >> packet.m_zoneFlag;
	return in;
}

QDataStream &operator>>(QDataStream &in, PacketSessionData &packet) {
	in >> packet.m_weather >> packet.m_trackTemperature >> packet.m_airTemperature >> packet.m_totalLaps >>
	packet.m_trackLength >> packet.m_sessionType >> packet.m_trackId >> packet.m_formula >> packet.m_sessionTimeLeft >>
	packet.m_sessionDuration >> packet.m_pitSpeedLimit >> packet.m_gamePaused >> packet.m_isSpectating >>
	packet.m_spectatorCarIndex >> packet.m_sliProNativeSupport >> packet.m_numMarshalZones;
	readDataList<MarshalZone>(in, packet.m_marshalZones, 21);
	in >> packet.m_safetyCarStatus >> packet.m_networkGame;
	return in;
}

QDataStream &operator>>(QDataStream &in, CarStatusData &packet) {
	in >> packet.m_tractionControl >> packet.m_antiLockBrakes >> packet.m_fuelMix >> packet.m_frontBrakeBias >>
	packet.m_pitLimiterStatus >> packet.m_fuelInTank >> packet.m_fuelCapacity >> packet.m_fuelRemainingLaps >>
	packet.m_maxRPM >> packet.m_idleRPM >> packet.m_maxGears >> packet.m_drsAllowed;
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_tyresWear[i];
	in >> packet.m_tyreCompound >> packet.m_tyreVisualCompound;
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_tyresDamage[i];
	in >> packet.m_frontLeftWingDamage >> packet.m_frontRightWingDamage >> packet.m_rearWingDamage >> packet.m_engineDamage >>
	packet.m_gearBoxDamage >> packet.m_vehicleFiaFlags >> packet.m_ersStoreEnergy >> packet.m_ersDeployMode >>
	packet.m_ersHarvestedThisLapMGUK >> packet.m_ersHarvestedThisLapMGUH >> packet.m_ersDeployedThisLap;
	return in;
}

QDataStream &operator>>(QDataStream &in, PacketCarStatusData &packet) {
	readDataList<CarStatusData>(in, packet.m_carStatusData);
	return in;
}

QDataStream &operator>>(QDataStream &in, PacketMotionData &packet) {
	readDataList<CarMotionData>(in, packet.m_carMotionData);

	for(auto i = 0; i < 4; ++i)
		in >> packet.m_suspensionPosition[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_suspensionVelocity[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_suspensionAcceleration[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_wheelSpeed[i];
	for(auto i = 0; i < 4; ++i)
		in >> packet.m_wheelSlip[i];

	in >> packet.m_localVelocityX >> packet.m_localVelocityY >> packet.m_localVelocityZ >> packet.m_angularVelocityX >>
	packet.m_angularVelocityY >> packet.m_angularVelocityZ >> packet.m_angularAccelerationX >>
	packet.m_angularAccelerationY >> packet.m_angularAccelerationZ >> packet.m_frontWheelsAngle;

	return in;
}

QDataStream &operator>>(QDataStream &in, CarMotionData &packet) {
	in >> packet.m_worldPositionX >> packet.m_worldPositionY >> packet.m_worldPositionZ >> packet.m_worldVelocityX >>
	packet.m_worldVelocityY >> packet.m_worldVelocityZ >> packet.m_worldForwardDirX >> packet.m_worldForwardDirY >>
	packet.m_worldForwardDirZ >> packet.m_worldRightDirX >> packet.m_worldRightDirY >> packet.m_worldRightDirZ >>
	packet.m_gForceLateral >> packet.m_gForceLongitudinal >> packet.m_gForceVertical >> packet.m_yaw >>
	packet.m_pitch >> packet.m_pitch;

	return in;
}

QDataStream &operator>>(QDataStream &in, PacketEventData &packet) {
	packet.m_eventStringCode.clear();
	char name[4];
	for(auto i = 0; i < 4; ++i) {
		qint8 c;
		in >> c;
		name[i] = c;
	}

	packet.m_eventStringCode = QString::fromUtf8(name);
    in >> packet.vehicleIdx >> packet.lapTime;
	packet.event = stringToEvent(packet.m_eventStringCode);
	return in;
}

Event stringToEvent(const QString str) {
	if(str.startsWith("SSTA"))
		return Event::SessionStarted;
	if(str.startsWith("SEND"))
		return Event::SessionEnded;
	if(str.startsWith("FTLP"))
		return Event::FastestLap;
	if(str.startsWith("RTMT"))
		return Event::Retirement;
	if(str.startsWith("DRSE"))
		return Event::DrsEnabled;
	if(str.startsWith("DRSD"))
		return Event::DrsDisabled;
	if(str.startsWith("TMPT"))
		return Event::TeammateInPits;
	if(str.startsWith("CHQF"))
		return Event::ChequeredFlag;
	if(str.startsWith("RCWN"))
		return Event::RaceWinner;

	return Event::Unknown;
}

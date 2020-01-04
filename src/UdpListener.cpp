#include <QDataStream>
#include <QNetworkDatagram>

#include "UdpListener.h"

F1Listener::F1Listener(F1PacketInterface *interface): _listener(new QUdpSocket(this)), _interface(interface) {

    int udp_port = 20777;

    // Bind to listening port and connect ready event to read method
    qDebug() << "Bind to listening port " << udp_port;
    _listener->bind(QHostAddress::Any, udp_port);
    connect(_listener, &QUdpSocket::readyRead, this, &F1Listener::readData);
}

bool F1Listener::tryRead() {

    // First read a "Header", then the data pakcet based on m_packetId
    // then Header, then data pakcet based on m_packetId...

	auto expectedLength = UdpSpecification::instance()->expectedPacketLength(_expectedDataType);
	if(_buffer.count() >= expectedLength) {
		auto dataToRead = _buffer.left(expectedLength);
		_buffer.remove(0, expectedLength);

		QDataStream stream(&dataToRead, QIODevice::ReadOnly);
		stream.setByteOrder(QDataStream::LittleEndian);
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

		switch(_expectedDataType) {

            case UdpSpecification::PacketType::Header: {
				stream >> _lastHeader;
                _interface->headerData(_lastHeader);
                // Header extracted,. Set the data type based on m_packetId
                _expectedDataType = static_cast<UdpSpecification::PacketType>(_lastHeader.m_packetId);
                return true;
			}

			case UdpSpecification::PacketType::Participants: {
				auto packet = PacketParticipantsData();
				stream >> packet;
                _interface->participantData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::LapData: {
				auto packet = PacketLapData();
				stream >> packet;
                _interface->lapData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::CarTelemetry: {
				auto packet = PacketCarTelemetryData();
				stream >> packet;
                _interface->telemetryData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::CarSetup: {
				auto packet = PacketCarSetupData();
				stream >> packet;
                _interface->setupData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::Session: {
				auto packet = PacketSessionData();
				stream >> packet;
                _interface->sessionData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::CarStatus: {
				auto packet = PacketCarStatusData();
				stream >> packet;
                _interface->statusData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::Motion: {
				auto packet = PacketMotionData();
				stream >> packet;
                _interface->motionData(_lastHeader, packet);
				break;
			}
			case UdpSpecification::PacketType::Event: {
				auto packet = PacketEventData();
				stream >> packet;
                _interface->eventData(_lastHeader, packet);
				break;
			}
			default:
				break;
		}

        // Now set the next packet type to "Header"
        _expectedDataType = UdpSpecification::PacketType::Header;

		return true;
	}

	return false;
}

void F1Listener::readData()
{
	while(_listener->hasPendingDatagrams()) {
		auto datagram = _listener->receiveDatagram();
		_buffer += datagram.data();
		while(tryRead()) {
		}
	}
}

F1PacketInterface::~F1PacketInterface() {}

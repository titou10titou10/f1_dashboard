#include "F1Listener.h"
#include <QDataStream>
#include <QNetworkDatagram>


F1Listener::F1Listener(F1PacketInterface *interface, QObject *parent)
: QObject(parent), _listener(new QUdpSocket(this)), _interface(interface) {

    qDebug() << "bind to listening port";

	// bind to listening port
	_listener->bind(QHostAddress::Any, 20777);

	connect(_listener, &QUdpSocket::readyRead, this, &F1Listener::readData);
}

bool F1Listener::tryRead() {

    // On lit 1 header, puis le reste des datas, puis un header, puis le reste des datas...
    // On attend que le nb de bytes soit present suivant le type de data

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
                // On a lu le header, on met le type de data qui suit
                qDebug() << "_expectedDataType: " << _lastHeader.m_packetId;
                _expectedDataType = static_cast<UdpSpecification::PacketType>(_lastHeader.m_packetId);
                return true;
                //break;
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


        //if(_expectedDataType == UdpSpecification::PacketType::Header)
        //	_expectedDataType = static_cast<UdpSpecification::PacketType>(_lastHeader.m_packetId);
        //else
        // on a lu les datas, on remet le type=Header
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

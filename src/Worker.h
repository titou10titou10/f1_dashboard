#ifndef F1LISTENER_H
#define F1LISTENER_H

#include "UdpSpecification.h"
#include <QObject>
#include <QUdpSocket>

class Worker : public QObject
{
	Q_OBJECT


  public:
    explicit Worker(F1PacketInterface *interface = nullptr, QObject *parent = nullptr);

  private:
	QUdpSocket *_listener = nullptr;
	QByteArray _buffer;
	F1PacketInterface *_interface;
	UdpSpecification::PacketType _expectedDataType = UdpSpecification::PacketType::Header;
	PacketHeader _lastHeader;

	bool tryRead();
	void readHeader(QByteArray &data);

  private slots:
	void readData();
};

#endif // F1LISTENER_H

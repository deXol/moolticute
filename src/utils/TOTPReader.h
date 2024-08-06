#ifndef TOTPREADER_H
#define TOTPREADER_H

#include <QObject>
#include "QZXing.h"

class TOTPReader : public QObject
{
    Q_OBJECT

public:
    TOTPReader(QObject *parent = nullptr);

    ~TOTPReader(){}

    static const int NOT_SET_VALUE = -1;

    struct TOTPResult {
        QString service = "";
        QString login = "";
        QString secret = "";
        int digits = NOT_SET_VALUE;
        int period = NOT_SET_VALUE;
        bool isValid = false;
    };

    static TOTPResult getQRFromFileDialog(QWidget* parent = nullptr);
    static TOTPResult getQRCodeResult(const QString& imgPath);
    static TOTPResult getQRCodeResult(const QImage& imgPath);


    static const QString TOTP_QR_WARNING;

private:
    static void setupDecoder();
    static TOTPResult processDecodedQR(const QString& res);

    static const int DEFAULT_DIGITS = 6;
    static const int DEFAULT_PERIOD = 30;
    static QZXing m_decoder;
    static bool m_qr_decoder_set;
    static const QString TOTP_URI;
    static const QString TOTP_HOST;
    static const QString SECRET;
    static const QString DIGITS;
    static const QString PERIOD;
};

#endif // TOTPREADER_H

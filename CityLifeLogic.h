#pragma once

#include <QDebug>

// Main
#include <QMap>
#include <QVector>
#include <QPair>

// For request
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>

// For JSON
#include <QByteArray>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

// For MD5
#include <QCryptographicHash>

class CityLifeLogic
{
    public:
        static const enum class MODE : int { TEST = 0, MAKE = 1, TRANSACTION = 2, STAT = 3, UNKNOWN = 4} REQUEST_MODE;

        // Keys for input args
        // Common
        static const QString KEY_DELIMETER;
        static const QString KEY_TIMEOUT;
        static const QString KEY_USER;
        static const QString KEY_PWD;
        static const QString KEY_OUTPUT;
        static const QString KEY_ERROR;
        static const QString KEY_MODE;
        static const QString KEY_ID;

        // For 1 and 2 mode
        static const QString KEY_TIME;
        static const QString KEY_START;
        static const QString KEY_FINAL;
        static const QString KEY_CURRENCY;
        static const QString KEY_DATA1;
        static const QString KEY_DATA2;
        static const QString KEY_DATA3;

        // For 4 mode
        static const QString KEY_TS;
        static const QString KEY_TE;

        // Keys for request
        // Common
        static const QString INIT_USER;
        static const QString INIT_ID;
        static const QString INIT_CRC;

        // For 1 and 2 mode
        static const QString INIT_TIME;
        static const QString INIT_START;
        static const QString INIT_FINAL;
        static const QString INIT_CURRENCY;
        static const QString INIT_DATA1;
        static const QString INIT_DATA2;
        static const QString INIT_DATA3;

        // For 4 mode
        static const QString INIT_TS;
        static const QString INIT_TE;

        // Input with request args
        static const QMap<QString, QString> INPUT_TO_OUTPUT;

        // Current args
        QVector<QPair<QString, QString>> * pContent;

	public:
		CityLifeLogic();
		~CityLifeLogic();

        // Send request to site
        bool sendRequest();
        // Get link for send request
        QString getLink();
        // Get MD5 hash for string
        QString getMd5(QByteArray data);
        // Set PWD
        void setPWD(QString password);
        // Set mode for send 'test' or 'make'
        void setMode(MODE mode);
        // Set path for file result, if it equal "", then without result output
        void setOutputPath(QString path);
        // Set path for file errors logging, if it equal "", then without logging
        void setErrorsPath(QString path);
        // Set delimeter for output result
        void setDelimeter(QString delimeter);
        // Set timeout in ms
        void setTimeout(int timeoutMs);
        // Fill containers
        static bool setValuesForRequest(QVector<QPair<QString, QString>> * arguments, QString * key, QString * value);
        // Get minimum arguments
        int getMinArg();
        // Output for log errors
        bool outputErrors(QString errors);
        // Output for result
        bool outputResult();
        // Get count folders of path
        QString getPath(QString path);

    private:
        // Finish args and values
        static QVector<QPair<QString, QString>> MAIN_ARGS_MODE_12;
        static QVector<QPair<QString, QString>> MAIN_ARGS_MODE_3;
        static QVector<QPair<QString, QString>> MAIN_ARGS_MODE_4;

        // For result
        QJsonArray result;
        // Type request
        QString typeRequest;
        // Password
        QString password;
        // Site name
        static const QString MAIN_SITE;
        // Request types
        static const QString TEST_PATH_SITE;
        static const QString MAKE_PATH_SITE;
        static const QString STAT_PATH_SITE;
        static const QString TRANSACTION_PATH_SITE;

        // Path to file of result data
        QString pathToOutput;
        // Path to file of errors
        QString pathToErrors;
        // For request mode
        MODE mode;
        // Minimun set for request
        int minArgs = 1;

        // Values for min input count
        static const int MIN_ARGS_MODE_12 = 7;
        static const int MIN_ARGS_MODE_3 = 2;
        static const int MIN_ARGS_MODE_4 = 3;

        // Delimeter
        QString delimeter;

        // Timeout delay
        int timeoutMs;
};


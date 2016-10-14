#include "CityLifeLogic.h"

// Input keys
const QString CityLifeLogic::KEY_TIMEOUT = "-TO";
const QString CityLifeLogic::KEY_DELIMETER = "-DL";
const QString CityLifeLogic::KEY_USER = "-U";
const QString CityLifeLogic::KEY_PWD = "-P";
const QString CityLifeLogic::KEY_OUTPUT = "-O";
const QString CityLifeLogic::KEY_MODE = "-M";
const QString CityLifeLogic::KEY_ERROR = "-E";

// For 1-2 mode
const QString CityLifeLogic::KEY_ID = "-I";
const QString CityLifeLogic::KEY_TIME = "-T";
const QString CityLifeLogic::KEY_START = "-S";
const QString CityLifeLogic::KEY_FINAL = "-F";
const QString CityLifeLogic::KEY_CURRENCY = "-C";
const QString CityLifeLogic::KEY_DATA1 = "-D1";
const QString CityLifeLogic::KEY_DATA2 = "-D2";
const QString CityLifeLogic::KEY_DATA3 = "-D3";

// For 4 mode
const QString CityLifeLogic::KEY_TS = "-TS";
const QString CityLifeLogic::KEY_TE = "-TE";

// Output keys
const QString CityLifeLogic::INIT_USER = "user";
const QString CityLifeLogic::INIT_CRC = "crc";

// For 1-2 mode
const QString CityLifeLogic::INIT_ID = "id";
const QString CityLifeLogic::INIT_TIME = "time";
const QString CityLifeLogic::INIT_START = "start";
const QString CityLifeLogic::INIT_FINAL = "final";
const QString CityLifeLogic::INIT_CURRENCY = "currency";
const QString CityLifeLogic::INIT_DATA1 = "data1";
const QString CityLifeLogic::INIT_DATA2 = "data2";
const QString CityLifeLogic::INIT_DATA3 = "data3";

// For 4 mode
const QString CityLifeLogic::INIT_TS = "ts_start";
const QString CityLifeLogic::INIT_TE = "ts_end";

// Short input keys to full for request
const QMap<QString, QString> CityLifeLogic::INPUT_TO_OUTPUT {
                                                                { CityLifeLogic::KEY_USER, CityLifeLogic::INIT_USER },
                                                                { CityLifeLogic::KEY_ID, CityLifeLogic::INIT_ID },
                                                                { CityLifeLogic::KEY_TIME, CityLifeLogic::INIT_TIME },
                                                                { CityLifeLogic::KEY_START, CityLifeLogic::INIT_START },
                                                                { CityLifeLogic::KEY_FINAL, CityLifeLogic::INIT_FINAL },
                                                                { CityLifeLogic::KEY_CURRENCY, CityLifeLogic::INIT_CURRENCY },
                                                                { CityLifeLogic::KEY_DATA1, CityLifeLogic::INIT_DATA1 },
                                                                { CityLifeLogic::KEY_DATA2, CityLifeLogic::INIT_DATA2 },
                                                                { CityLifeLogic::KEY_DATA3, CityLifeLogic::INIT_DATA3 },
                                                                { CityLifeLogic::KEY_TS, CityLifeLogic::INIT_TS },
                                                                { CityLifeLogic::KEY_TE, CityLifeLogic::INIT_TE }
                                                            };

QVector<QPair<QString, QString>> CityLifeLogic::MAIN_ARGS_MODE_12{
                                                                    { CityLifeLogic::INIT_ID, "" },
                                                                    { CityLifeLogic::INIT_TIME, "" },
                                                                    { CityLifeLogic::INIT_START, "" },
                                                                    { CityLifeLogic::INIT_FINAL, "" },
                                                                    { CityLifeLogic::INIT_CURRENCY, "" },
                                                                    { CityLifeLogic::INIT_DATA1, "" },
                                                                    { CityLifeLogic::INIT_DATA2, "" },
                                                                    { CityLifeLogic::INIT_DATA3, "" },
                                                                    { CityLifeLogic::INIT_USER, "" }
                                                                };

QVector<QPair<QString, QString>> CityLifeLogic::MAIN_ARGS_MODE_3{
                                                                    { CityLifeLogic::INIT_ID, "" },
                                                                    { CityLifeLogic::INIT_USER, "" }
                                                                };

QVector<QPair<QString, QString>> CityLifeLogic::MAIN_ARGS_MODE_4{
                                                                    { CityLifeLogic::INIT_TS, "" },
                                                                    { CityLifeLogic::INIT_TE, "" },
                                                                    { CityLifeLogic::INIT_USER, "" }
                                                                };

const QString CityLifeLogic::MAIN_SITE = "https://clcorp.ru:8282/op/";
const QString CityLifeLogic::TEST_PATH_SITE = "test?";
const QString CityLifeLogic::MAKE_PATH_SITE = "make?";
const QString CityLifeLogic::TRANSACTION_PATH_SITE = "transactionInfo?";
const QString CityLifeLogic::STAT_PATH_SITE = "stat?";

CityLifeLogic::CityLifeLogic(){
    pathToOutput = "";
    mode = CityLifeLogic::MODE::UNKNOWN;
    pContent = NULL;
    password = "";
    delimeter = " :: ";
    timeoutMs = 5000;
}

CityLifeLogic::~CityLifeLogic(){

}

QString CityLifeLogic::getMd5(QByteArray data){
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
}

bool CityLifeLogic::sendRequest(){
    if(mode == CityLifeLogic::MODE::UNKNOWN)
        return false;

    // For abort when timeout
    QTimer timer;
    timer.setSingleShot(true);

    // Create custom temporary event loop on stack
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    // Request object
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "OPBConsoleAppQt");

    // Get link for send
    QUrl sendURL(getLink());
    sendURL.setQuery(sendURL.query(QUrl::FullyEncoded));
    request.setUrl(sendURL);

    // SSL configuration
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(config);
    request.setSslConfiguration(config);

    // Run timer
    timer.start(timeoutMs);
    // Send request
    QNetworkReply * reply = mgr.get(request);
    reply->ignoreSslErrors();
    // Run loop and wait result. BLOCK APP.
    eventLoop.exec();

    // If timer still work, but reques finish, else if timer finished first disconnect it
    if(timer.isActive()){
        timer.stop();
        // Bad request
        if (reply->error() != QNetworkReply::NoError){
            QString returnData(reply->readAll());
            qDebug() << "Error for URL:" << "\n";
            qDebug() << reply->errorString() << "\n";
            qDebug() << "Return data:" << "\n";
            qDebug() << returnData << "\n";
            outputErrors("Error for URL:");
            outputErrors(reply->errorString());
            outputErrors(returnData);
            delete reply;
            return false;
        }
    } else {
       reply->abort();
       mgr.disconnect();
       qDebug() << "Error for server: Timeout" << "\n";
       outputErrors("Error for server: Timeout");
       delete reply;
       return false;
    }

    // Get data in array chars
    QByteArray byteArray(reply->readAll());
    // For check error during parse json
    QJsonParseError jsonParseError;
    // Try parse JSON
    QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray, &jsonParseError));

    // Check succes parsing
    if(jsonParseError.error != QJsonParseError::NoError){
        outputErrors("Error for JSON:");
        outputErrors(jsonParseError.errorString());
        outputErrors("JSON:");
        outputErrors(byteArray);
        delete reply;
        return false;
    }

    // Check type of JSON
    if(jsonDocument.isArray()){
        result = jsonDocument.array();
    } else if(jsonDocument.isObject()){
        result.append(jsonDocument.object());
    }

    delete reply;
    return true;
}

QString CityLifeLogic::getLink(){
    QString compliteLink(CityLifeLogic::MAIN_SITE);
            compliteLink.append(typeRequest);
    QString stringForHash;

    // Iterate all arguments and concat in one string for URL
    if(pContent != NULL)
        for(int i = 0; i < pContent->size(); i++){
            compliteLink.append(pContent->at(i).first).append("=").append(pContent->at(i).second.toHtmlEscaped()).append("&");
            stringForHash.append(pContent->at(i).second).append("*");
        }

    // String for hash ends by passsword!
    stringForHash.append(password);
    // Get last element for link - hash
    compliteLink.append(CityLifeLogic::INIT_CRC).append("=").append(getMd5(stringForHash.toUtf8()));
    return compliteLink;
}

void CityLifeLogic::setMode(MODE mode){
    this->mode = mode;
    switch(mode){
        case CityLifeLogic::MODE::TEST :
            typeRequest = CityLifeLogic::TEST_PATH_SITE;
            pContent = &MAIN_ARGS_MODE_12;
            minArgs = CityLifeLogic::MIN_ARGS_MODE_12;
            break;
        case CityLifeLogic::MODE::MAKE :
            typeRequest = CityLifeLogic::MAKE_PATH_SITE;
            pContent = &MAIN_ARGS_MODE_12;
            minArgs = CityLifeLogic::MIN_ARGS_MODE_12;
            break;
        case CityLifeLogic::MODE::TRANSACTION :
            typeRequest = CityLifeLogic::TRANSACTION_PATH_SITE;
            pContent = &MAIN_ARGS_MODE_3;
            minArgs = CityLifeLogic::MIN_ARGS_MODE_3;
            break;
        case CityLifeLogic::MODE::STAT :
            typeRequest = CityLifeLogic::STAT_PATH_SITE;
            pContent = &MAIN_ARGS_MODE_4;
            minArgs = CityLifeLogic::MIN_ARGS_MODE_4;
            break;
    }
}

void CityLifeLogic::setOutputPath(QString path){
  this->pathToOutput = path;
}

void CityLifeLogic::setErrorsPath(QString path){
  this->pathToErrors = path;
}

int CityLifeLogic::getMinArg(){
    return minArgs;
}

void CityLifeLogic::setPWD(QString password){
    this->password = password;
}

void CityLifeLogic::setDelimeter(QString delimeter){
    if(delimeter.trimmed().compare("") != 0)
        this->delimeter = " " + delimeter.trimmed() + " ";
}

void CityLifeLogic::setTimeout(int timeoutMs){
    if(timeoutMs >= 1000 && timeoutMs <= 10000)
        this->timeoutMs = timeoutMs;
}

bool CityLifeLogic::setValuesForRequest(QVector<QPair<QString, QString>> * arguments, QString * key, QString * value){
    for(int i = 0; i < arguments->size(); i++){
        if(CityLifeLogic::INPUT_TO_OUTPUT.contains(*key))
            if(CityLifeLogic::INPUT_TO_OUTPUT.value(*key).compare(arguments->at(i).first) == 0){
                (*arguments)[i].second = *value;
                return true;
            }
    }

    return false;
}

bool CityLifeLogic::outputErrors(QString errors){
    if(pathToErrors.compare("") != 0 && errors.compare("") != 0){
        QFile file(pathToErrors);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            return false;

         QTextStream out(&file);
         QDateTime currentTime(QDateTime::currentDateTimeUtc());
         currentTime = currentTime.addSecs(3600 * 3);
         out << currentTime.toString("dd.MM.yyyy-HH:MM:ss") << " : " << errors << "\n";
         file.flush();
         file.close();
         return true;
    }

    return false;
}

bool CityLifeLogic::outputResult(){
    if(pathToOutput.compare("") != 0 && result.size() > 0){
        QFile file(pathToOutput);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            return false;

         QTextStream out(&file);
         // Iterate return data and out to file
         foreach (const QJsonValue & item, result){
             QStringList keys = item.toObject().keys();
             foreach (const QString key, keys){
                 out << key << delimeter << item.toObject().value(key).toString() << "\n";
             }
             out << "\n";
         }

         file.flush();
         file.close();
         return true;
    }

    return false;
}

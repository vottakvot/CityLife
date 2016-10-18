#include <QCoreApplication>
#include <QHash>
#include <QMap>
#include <QPair>
#include <QException>
#include <exception>
#include <iostream>
#include "CityLifeLogic.h"

using namespace std;

static const int RET_VALUE_OK = 0;
static const int RET_VALUE_UNKNOWN = 1;
static const int RET_VALUE_WRONG_ARGS = 2;
static const int RET_VALUE_BAD_REQUEST = 3;
static const int RET_VALUE_ERROR_OUTPUT_FILE = 4;

void help();
int getArgs(int argc, char *argv[]);
CityLifeLogic cityLifeLogic;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    try {
        // Check all args
        if (getArgs(argc, argv) < cityLifeLogic.getMinArg())
            throw RET_VALUE_WRONG_ARGS;

        // Send request
        if(!cityLifeLogic.sendRequest())
            throw RET_VALUE_BAD_REQUEST;

        // Write result
        if(!cityLifeLogic.outputResult())
            throw RET_VALUE_ERROR_OUTPUT_FILE;

    } catch (int e) {
        help();
        a.exit(e);
        return e;
    } catch (QException &e){
        cout << "Unknown error with Qt. Code: " << e.what() << endl << endl;
        help();
        a.exit(RET_VALUE_UNKNOWN);
        return RET_VALUE_UNKNOWN;
    } catch (exception &e) {
        cout << "Unknown error. Code: " << e.what() << endl << endl;
        help();
        a.exit(RET_VALUE_UNKNOWN);
        return RET_VALUE_UNKNOWN;
    }

    a.exit(RET_VALUE_OK);
    return RET_VALUE_OK;
}

void help(){
    cout << "CityLife console application help." << endl;
    cout << "    Common keys:" << endl;
    cout << "       -O - for output result in file" << endl;
    cout << "       -E - for logging errors" << endl;
    cout << "       -M - mode for send \"1\" - test or \"2\" - make or \"3\" - transaction or \"4\" - stat" << endl;
    cout << "       -U - user name for shop" << endl;
    cout << "       -P - user password. MD5 gets by password! Without it MD5 will wrong!" << endl;
    cout << "       -DL - delimeter for output results. Default = \" :: \"." << endl;
    cout << "    Keys for mode \"test\" = 1 or \"make\" = 2:" << endl;
    cout << "       -I - id of current transaction" << endl;
    cout << "       -T - current time. Format \"YYYY-MM-DD hh:mm:ss\"" << endl;
    cout << "       -S - start price, without discount" << endl;
    cout << "       -F - price with discount" << endl;
    cout << "       -C - currency type" << endl;
    cout << "       -D1 - first part of user card number" << endl;
    cout << "       -D2 - add part of number" << endl;
    cout << "       -D3 - add part of number" << endl;
    cout << "    Example:" << endl;
    cout << "       -M 1 -I TEST_1 -T \"2016-10-01 14:03:03\" -S 10 -F 8 -C RUR -D1 2810824457731 -U user -P pwd -O result.txt" << endl << endl;
    cout << "    Keys for mode \"transaction\":" << endl;
    cout << "       -I - id of transaction" << endl;
    cout << "    Example:" << endl;
    cout << "       -M 3 -I TEST_1 -U user -P pwd -O result.txt" << endl << endl;
    cout << "    Keys for mode \"stat\":" << endl;
    cout << "       -TS - begin period" << endl;
    cout << "       -TE - end period" << endl;
    cout << "    Example:" << endl;
    cout << "       -M 4 -TS \"2016-10-01 14:03:03\" -TE \"2016-10-10 14:03:03\" -U user -P pwd -O result.txt" << endl << endl;
}

int getArgs(int argc, char *argv[]){
    // For search '-M' argument. If it not exist or wrong value - exit app
    bool isMKey = false;
    for(int i = 0; i < argc - 1; i++){
        QString key(argv[i]), value(argv[i + 1]);
        if(CityLifeLogic::KEY_MODE.compare(key) == 0){
            switch((CityLifeLogic::MODE)value.toInt()) {
                case CityLifeLogic::MODE::MAKE : case CityLifeLogic::MODE::TEST : case CityLifeLogic::MODE::TRANSACTION :
                case CityLifeLogic::MODE::STAT : cityLifeLogic.setMode(static_cast<CityLifeLogic::MODE>(value.toInt()));
                    break;
                default:
                    return 0;
            }
            isMKey = true;
            break;
        }
    }

    // If '-M' arg doesn't exist
    if(!isMKey)
        return 0;

    // Check count args, if less then exit
    int countArgs = 0;

    // Search for other arguments
    for (int i = 0; i < argc - 1; i++){
        QString key(argv[i]), value(argv[i + 1]);
        if (CityLifeLogic::INPUT_TO_OUTPUT.contains(key)){
            if(CityLifeLogic::setValuesForRequest(cityLifeLogic.pContent, &key, &value))
                countArgs++;
        } else if(CityLifeLogic::KEY_OUTPUT.compare(key) == 0){
                cityLifeLogic.setOutputPath(value);
        } else if(CityLifeLogic::KEY_ERROR.compare(key) == 0){
            cityLifeLogic.setErrorsPath(value.trimmed());
        } else if(CityLifeLogic::KEY_PWD.compare(key) == 0){
            cityLifeLogic.setPWD(value.trimmed());
        } else if(CityLifeLogic::KEY_DELIMETER.compare(key) == 0){
            cityLifeLogic.setDelimeter(value.trimmed());
        } else if(CityLifeLogic::KEY_TIMEOUT.compare(key) == 0){
            cityLifeLogic.setTimeout(value.trimmed().toInt() * 1000);
        }
    }

    return countArgs;
}

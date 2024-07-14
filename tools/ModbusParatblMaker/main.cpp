#include "mainwindow.h"

#include <QApplication>

#include "xlsxdocument.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <QMap>

#include <QSqlQuery>
#include <QtSql>
#include <qsqldatabase.h>

#define PATH  qApp->applicationDirPath() +"/"
// #define PATH QString("F:/gsdk/tools/ModbusParatblMaker/")
// #define PATH QString("F:/gsdk/examples/stm32hal_g070rb_Servo/Database/")

// #define PATH_DST    QString("C:/Program Files (x86)/co-trust/MagicWorks Tuner V1.6/")
#define PATH_DST       QString("C:/Program Files (x86)/co-trust/MagicWorks Tuner debug_V1.81A_0104/Device/")

#define CONFIG_DATABASE_SW      1

#define PARA_COLUMN_OFFSET      ('D' - 'A' + 1)

#define PARA_ADDRESS            (PARA_COLUMN_OFFSET + 0)
#define PARA_DATA_TYPE          (PARA_COLUMN_OFFSET + 1)
#define PARA_VAR_NAME           (PARA_COLUMN_OFFSET + 2)
#define PARA_ATTR_INTI_VAL      (PARA_COLUMN_OFFSET + 3)
#define PARA_ATTR_MIN_VAL       (PARA_COLUMN_OFFSET + 4)
#define PARA_ATTR_MAX_VAL       (PARA_COLUMN_OFFSET + 5)
#define PARA_SUBATTR_UNIT       (PARA_COLUMN_OFFSET + 6)
#define PARA_SUBATTR_MODE       (PARA_COLUMN_OFFSET + 7)
#define PARA_SUBATTR_RELATE     (PARA_COLUMN_OFFSET + 8)
#define PARA_SUBATTR_SYNC_COVER (PARA_COLUMN_OFFSET + 9)
#define PARA_SUBATTR_ACCESS     (PARA_COLUMN_OFFSET + 10)
#define PARA_OBJECT_DICT        (PARA_COLUMN_OFFSET + 11)
#define PARA_NAME_ZH            (PARA_COLUMN_OFFSET + 12)
#define PARA_DESC_ZH            (PARA_COLUMN_OFFSET + 13)

#if CONFIG_DATABASE_SW

QSqlDatabase db;

bool DB_Init()
{
    qDebug() << QSqlDatabase::drivers();

    QFile::remove(PATH + "default.db");
    QFile::remove(PATH_DST + "default.db");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(PATH + "default.db");  // create if not exist

    bool isok = db.open();

    if (isok == false)
    {
        qDebug() << "error info :" << db.lastError();
    }
    else
    {
        QSqlQuery query(db);

        QString creatTableStr = "CREATE TABLE %1   \
                    (                                \
                        Address INTEGER PRIMARY KEY, \
                        DataType TEXT,               \
                        ParaName_CN TEXT,            \
                        ControlMode TEXT,            \
                        LowerLimit TEXT,             \
                        UpperLimit TEXT,             \
                        DefaultValue TEXT,           \
                        ReadWrite TEXT,              \
                        Comment_CN TEXT,             \
                        ParaName_EN TEXT,            \
                        Comment_EN TEXT              \
                    );";

        creatTableStr = creatTableStr.arg("Para_A4");  // set table name

        query.prepare(creatTableStr);

        if (!query.exec())
        {
            qDebug() << "query error :" << query.lastError();
        }
        else
        {
            qDebug() << "creat table success!";
        }
    }

    return isok;
}

void DB_Deinit()
{
    db.close();
    QFile::copy(PATH + "default.db", PATH_DST + "default.db");
}

#else

bool DB_Init()
{
    return true;
}

void DB_Deinit() {}

#endif

void MakePara(
    QString      szAddress,
    QString      szDataType,
    QString      szVarName,
    QString      szParaNameZH,
    QString      szInitVal,
    QString      szMinVal,
    QString      szMaxVal,
    QString      szMode,
    QString      szRelate,
    QString      szSynCov,
    QString      szAccess,
    QString      szParaNameDescZH,
    QTextStream& ParaTable,
    QTextStream& ParaAttr)
{
    // qDebug() << szAddress << szDataType << szVarName << szInitVal << szMinVal << szMaxVal << szMode << szRelate << szSynCov << szAccess << Qt::endl;

    QString     szSubAttr;
    QTextStream subStream(&szSubAttr);

    // clang-format off

    QMap<QString, QString> a = {
        {"只读",                    "   B_RO"},
        {"读写 - 立即更改，立即生效", "B_RW_M0"},
        {"读写 - 立即更改，重启生效", "B_RW_M1"},
        {"读写 - 停机更改，立即生效", "B_RW_M2"},
    };

    QMap<QString, QString> b = {
        {"允许保存，可恢复",     " B_SYNC |  B_COV"},
        {"允许保存，不可恢复",   " B_SYNC | B_NCOV"},
        {"不允许保存，不可恢复", "B_NSYNC | B_NCOV"},
    };

    QMap<QString, QString> d = {
        {"允许保存，可恢复",     " B_SYNC |  B_COV"},
        {"允许保存，不可恢复",   " B_SYNC | B_NCOV"},
        {"不允许保存，不可恢复", "B_NSYNC | B_NCOV"},
    };

    QMap<QString, QString> c = {
        {"无效"  , "   B_NL"},
        {"仅上限", "B_RL_UP"},
        {"仅下限", "B_RL_DN"},
        {"上下限", "   B_RL"},
    };

    // clang-format on

    szMode   = a.value(szMode);
    szSynCov = b.value(szSynCov);
    szRelate = c.value(szRelate);

    QStringList WordInd1, WordInd2;

    if (szDataType == "s16" || szDataType == "u16")
    {
        WordInd1 << "";
        WordInd2 << " B_SIG";
    }
    else if (szDataType == "s32" || szDataType == "u32")
    {
        WordInd1 << "WL" << "WH";
        WordInd2 << "B_DOB0" << "B_DOB1";
    }
    else if (szDataType == "s64" || szDataType == "u64")
    {
        WordInd1 << "W0" << "W1" << "W2" << "W3";
        WordInd2 << "B_QUD0" << "B_QUD1" << "B_QUD2" << "B_QUD3";
    }

    if (szVarName == "")
    {
        szVarName = "_Resv" + szAddress;
    }

    szAddress = QString("%1").arg(szAddress.toUInt(), 4, 10, QLatin1Char('0'));

    QString styleVar = "    %1 %2 // P%3 %4";

    QString styleAttr = "    { %1, %2, %3, %4 | %5 | %6 | %7 }, // P%8 %9";

    for (uint8_t i = 0; i < WordInd1.size(); ++i)
    {
        ParaTable << styleVar
                         .arg(szDataType)
                         .arg(szVarName + ";", -20)  // <0: 左对齐, >0 右对齐
                         .arg(szAddress)
                         .arg(szParaNameZH)
                  << Qt::endl;

        ParaAttr << styleAttr
                        .arg(WordInd1[i] + "(" + szInitVal + ")", 15)
                        .arg(WordInd1[i] + "(" + szMinVal + ")", 15)
                        .arg(WordInd1[i] + "(" + szMaxVal + ")", 15)
                        .arg(szMode)
                        .arg(szRelate)
                        .arg(szSynCov)
                        .arg(WordInd2[i])
                        .arg(szAddress)
                        .arg(szVarName + WordInd1[i])
                 << Qt::endl;
    }

#if CONFIG_DATABASE_SW

    QSqlQuery query(db);

    QString cmd = "replace into %1 values(\"%2\",%3,\"%4\",\"0\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",\"\",\"\");";

    cmd = cmd.arg("Para_A4")  // table name
              .arg(szAddress)
              .arg(WordInd1.size())
              .arg(szVarName + " | " + szParaNameZH)
              .arg(szMinVal)
              .arg(szMaxVal)
              .arg(szInitVal)
              .arg("")
              .arg(szParaNameDescZH);

    query.prepare(cmd);
    query.exec();

#endif
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QXlsx::Document xlsx(PATH + "paratbl.xlsx");

    QFile       file1(PATH + "paraattr.c");
    QTextStream ParaAttr(&file1);

    QFile       file2(PATH + "paratbl.h");
    QTextStream ParaTbl(&file2);

    bool isok = DB_Init() && file1.open(QIODevice::WriteOnly) && file2.open(QIODevice::WriteOnly);

    if (isok)
    {
        ParaAttr.setCodec("UTF-8");
        ParaTbl.setCodec("UTF-8");

        ParaAttr << "// clang-format off" << Qt::endl;
        ParaAttr << "para_attr_t sParaAttr = {" << Qt::endl;

        ParaTbl << "// clang-format off" << Qt::endl;
        ParaTbl << "typedef struct __packed {" << Qt::endl;

        for (quint16 row = 6; row < 700; row++)
        {
            QString szAddress = xlsx.cellAt(row, PARA_ADDRESS)->value().toString();

            if (szAddress.isEmpty())
            {
                break;
            }

             // qDebug() << szAddress;

            MakePara(szAddress,
                     xlsx.cellAt(row, PARA_DATA_TYPE)->value().toString(),
                     xlsx.cellAt(row, PARA_VAR_NAME)->value().toString(),
                     xlsx.cellAt(row, PARA_NAME_ZH)->value().isNull() ? "":xlsx.cellAt(row, PARA_NAME_ZH)->value().toString(),
                     xlsx.cellAt(row, PARA_ATTR_INTI_VAL)->value().toString(),
                     xlsx.cellAt(row, PARA_ATTR_MIN_VAL)->value().toString(),
                     xlsx.cellAt(row, PARA_ATTR_MAX_VAL)->value().toString(),
                     xlsx.cellAt(row, PARA_SUBATTR_MODE)->value().toString(),
                     xlsx.cellAt(row, PARA_SUBATTR_RELATE)->value().toString(),
                     xlsx.cellAt(row, PARA_SUBATTR_SYNC_COVER)->value().toString(),
                     xlsx.cellAt(row, PARA_SUBATTR_ACCESS)->value().toString(),
                     xlsx.cellAt(row, PARA_DESC_ZH)->value().isNull() ? "":xlsx.cellAt(row, PARA_DESC_ZH)->value().toString(),
                     ParaTbl, ParaAttr);
        }

        ParaAttr << "} para_attr_t;" << Qt::endl;
        ParaAttr << "// clang-format on" << Qt::endl;

        ParaTbl << "} para_table_t;" << Qt::endl;
        ParaTbl << "// clang-format on" << Qt::endl;
    }

    DB_Deinit();

    // MainWindow w;
    // w.show();
    // return a.exec();

    return 0;
}

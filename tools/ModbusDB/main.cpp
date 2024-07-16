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
#include <QMessageBox>

#include "xlsxworkbook.h"
#include "QDesktopServices"

#define PATH qApp->applicationDirPath() + "/"
// #define PATH QString("F:/gsdk/tools/ModbusParatblMaker/")
#define PATH QString("F:/gsdk/examples/stm32hal_g070rb_Servo/Database/")

// #define PATH_DST    QString("C:/Program Files (x86)/co-trust/MagicWorks Tuner V1.6/")
#define PATH_DST                QString("C:/Program Files (x86)/co-trust/MagicWorks Tuner debug_V1.81A_0104/Device/")

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

bool IsProcessExist(const QString &processName)
{
    QProcess process;

    process.start("tasklist");
    process.waitForFinished();

    QByteArray result = process.readAllStandardOutput();
    QString str = result;

    return str.contains(processName);
}

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
        {"无效"  , "   B_NR"},
        {"仅上限", "B_RL_UP"},
        {"仅下限", "B_RL_DN"},
        {"上下限", "   B_RL"},
    };

    // clang-format on

    szMode   = a.value(szMode);
    szSynCov = b.value(szSynCov);
    szRelate = c.value(szRelate);

    QStringList WordInd1, WordInd2, Suffix;

    if (szDataType == "s16" || szDataType == "u16")
    {
        WordInd1 << "W";
        WordInd2 << " B_SIG";
        Suffix << "";
    }
    else if (szDataType == "s32" || szDataType == "u32")
    {
        WordInd1 << "WL" << "WH";
        WordInd2 << "B_DOB0" << "B_DOB1";
        Suffix = WordInd1;
    }
    else if (szDataType == "s64" || szDataType == "u64")
    {
        WordInd1 << "W0" << "W1" << "W2" << "W3";
        WordInd2 << "B_QUD0" << "B_QUD1" << "B_QUD2" << "B_QUD3";
        Suffix = WordInd1;
    }

    if (szVarName == "")
    {
        szVarName = "_Resv" + szAddress;
    }

    szAddress = QString("%1").arg(szAddress.toUInt(), 4, 10, QLatin1Char('0'));

    QString styleVar = "    %1 %2 // P%3 %4";

    ParaTable << styleVar
                     .arg(szDataType)
                     .arg(szVarName + ";", -20)  // <0: 左对齐, >0 右对齐
                     .arg(szAddress)
                     .arg(szParaNameZH)
              << Qt::endl;

    QString styleAttr = "    { %1, %2, %3, %4 | %5 | %6 | %7 }, // P%8 %9";

    for (uint8_t i = 0; i < WordInd1.size(); ++i)
    {
        ParaAttr << styleAttr
                        .arg(WordInd1[i] + "(" + szInitVal + ")", 15)
                        .arg(WordInd1[i] + "(" + szMinVal + ")", 15)
                        .arg(WordInd1[i] + "(" + szMaxVal + ")", 15)
                        .arg(szMode)
                        .arg(szRelate)
                        .arg(szSynCov)
                        .arg(WordInd2[i])
                        .arg(szAddress)
                        .arg(szVarName + Suffix[i])
                 << Qt::endl;
    }

#if CONFIG_DATABASE_SW

    QSqlQuery query(db);

    QString cmd = "replace into %1 values(\"%2\",%3,\"%4\",\"0\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",\"\",\"\");";

    cmd = cmd.arg("Para_A4")  // table name
              .arg(szAddress)
              .arg(WordInd1.size()*4) // half byte count: word=4,dword=8
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

bool MergeFile(QString szMiddle /* dst */, QString szFront, QString szBack)
{
    QFile f1(szFront);
    QFile f2(szMiddle);
    QFile f3(szBack);

    QTextStream s1(&f1);
    QTextStream s2(&f2);
    QTextStream s3(&f3);

    s1.setCodec("UTF-8");
    s2.setCodec("UTF-8");
    s3.setCodec("UTF-8");

    QString ctx;

    if (f2.open(QIODevice::ReadOnly) == false)
    {
        return false;
    }

    if (f1.open(QIODevice::ReadOnly) == true)
    {
        ctx += s1.readAll();
        f1.close();
    }

    ctx += s2.readAll();
    f2.close();

    if (f3.open(QIODevice::ReadOnly) == true)
    {
        ctx += s3.readAll();
        f3.close();
    }

    if (f2.open(QIODevice::WriteOnly) == true)
    {
        s2 << ctx;
    }

    return true;
}

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    QXlsx::Document xlsx(PATH + "paratbl.xlsx");
    QXlsx::Workbook *workBook = xlsx.workbook();

    QFile       ParaAttrFile(PATH + "paraattr.c");
    QTextStream ParaAttrStream(&ParaAttrFile);

    QFile       ParaTblFile(PATH + "paratbl.h");
    QTextStream ParaTblStream(&ParaTblFile);

    bool bIsOk = DB_Init() && ParaAttrFile.open(QIODevice::WriteOnly) && ParaTblFile.open(QIODevice::WriteOnly);

    if(bIsOk == false)
    {
        QMessageBox::warning(Q_NULLPTR, "ModbusDB", "fail to open file");
        return 0;
    }

    bool IsTunerRunning = IsProcessExist("MagicWorksTuner.exe");

    if( IsTunerRunning )
    {
        QProcess::execute("taskkill /im MagicWorksTuner.exe /f"); // 关闭进程
    }

    ParaAttrStream.setCodec("UTF-8");
    ParaTblStream.setCodec("UTF-8");

    for(uint8_t i=0; i < workBook->sheetCount(); ++i)
    {
        QXlsx::Worksheet *workSheet = static_cast<QXlsx::Worksheet*>(workBook->sheet(i));

        QString ParaTblHdr, ParaTblTail;
        QString ParaAttrHdr, ParaAttrTail;

        if( workSheet->sheetName() == "Device" ||
            workSheet->sheetName() == "Motor" ||
            workSheet->sheetName() == "Axis" )
        {

        }
        else
        {
            continue;
        }

        ParaTblHdr = "\ntypedef struct __packed {\n";
        ParaTblTail = QString("} %1_para_t;\n").arg(workSheet->sheetName().toLower());

        ParaAttrHdr = QString("\nconst para_attr_t s%1Attr[] = {\n").arg(workSheet->sheetName());
        ParaAttrTail = "};\n";

        ParaTblStream << ParaTblHdr;
        ParaAttrStream << ParaAttrHdr;

//        qDebug() << workSheet->dimension().rowCount();

        for (quint16 row = 5; row < workSheet->dimension().rowCount(); row++)
        {

            if (workSheet->cellAt(row, PARA_ADDRESS)->value().isValid() == false ||
                workSheet->cellAt(row, PARA_ADDRESS)->value().toString().isEmpty())
            {
                break;
            }

#if 0
            qDebug() << workSheet->sheetName()
                     << workSheet->cellAt(row, PARA_ADDRESS)->value().toString()
                     << workSheet->cellAt(row, PARA_VAR_NAME)->value().toString();
#endif

            MakePara(workSheet->cellAt(row, PARA_ADDRESS)->value().toString(),
                     workSheet->cellAt(row, PARA_DATA_TYPE)->value().toString(),
                     workSheet->cellAt(row, PARA_VAR_NAME)->value().toString(),
                     !workSheet->cellAt(row, PARA_NAME_ZH)->value().isValid() ? "" : workSheet->cellAt(row, PARA_NAME_ZH)->value().toString(),
                     workSheet->cellAt(row, PARA_ATTR_INTI_VAL)->value().toString(),
                     workSheet->cellAt(row, PARA_ATTR_MIN_VAL)->value().toString(),
                     workSheet->cellAt(row, PARA_ATTR_MAX_VAL)->value().toString(),
                     workSheet->cellAt(row, PARA_SUBATTR_MODE)->value().toString(),
                     workSheet->cellAt(row, PARA_SUBATTR_RELATE)->value().toString(),
                     workSheet->cellAt(row, PARA_SUBATTR_SYNC_COVER)->value().toString(),
                     workSheet->cellAt(row, PARA_SUBATTR_ACCESS)->value().toString(),
                     !workSheet->cellAt(row, PARA_DESC_ZH)->value().isValid() ? "" : workSheet->cellAt(row, PARA_DESC_ZH)->value().toString(),
                     ParaTblStream, ParaAttrStream);
        }

        ParaTblStream << ParaTblTail;
        ParaAttrStream << ParaAttrTail;
    }

    DB_Deinit();

    ParaAttrFile.close();
    ParaTblFile.close();

    MergeFile(PATH + "paraattr.c", PATH + "paraattr_front.c", PATH + "paraattr_back.c");
    MergeFile(PATH + "paratbl.h", PATH + "paratbl_front.h", PATH + "paratbl_back.h");

    QMessageBox::information(Q_NULLPTR, "ModbusDB", "- OK -");

    return 0;
}



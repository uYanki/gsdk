#include "mainwindow.h"

#include <QApplication>

#include "xlsxdocument.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

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

#define VALUE_WIDTH             15
#define ADDR_WIDTH              4

QString MakePara(QString szAddress, QString szDataType, QString szParaName, QString szInitVal, QString szMinVal, QString szMaxVal, QString szMode, QString szRelate, QString szSynCov, QString szAccess)
{
    QString     szContent;
    QTextStream stream(&szContent);

    qDebug() << szAddress << szDataType << szParaName << szInitVal << szMinVal << szMaxVal << szMode << szRelate << szSynCov << szAccess << Qt::endl;

    QString     szSubAttr;
    QTextStream subStream(&szSubAttr);

    if (szMode == "只读") { szMode = "   B_RO"; }
    else if (szMode == "读写 - 立即更改，立即生效") { szMode = "B_RW_M0"; }
    else if (szMode == "读写 - 立即更改，重启生效") { szMode = "B_RW_M1"; }
    else if (szMode == "读写 - 停机更改，立即生效") { szMode = "B_RW_M2"; }

    if (szSynCov == "允许保存，可恢复") { szSynCov = " B_SYNC |  B_COV"; }
    else if (szSynCov == "允许保存，不可恢复") { szSynCov = " B_SYNC | B_NCOV"; }
    else if (szSynCov == "不允许保存，不可恢复") { szSynCov = "B_NSYNC | B_NCOV"; }

    if (szRelate == "无效") { szRelate = "   B_NL"; }
    else if (szRelate == "仅上限") { szRelate = "B_RL_UP"; }
    else if (szRelate == "仅下限") { szRelate = "B_RL_DN"; }
    else if (szRelate == "上下限") { szRelate = "   B_RL"; }

    QString style = "    { %1, %2, %3, %4 | %5 | %6 | %7 }, // P%8 %9";

    if (szDataType == "s16" || szDataType == "u16")
    {
        stream << style
                      .arg("W(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("W(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("W(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg(" B_SIG")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName)
               << Qt::endl;
    }
    else if (szDataType == "s32" || szDataType == "u32")
    {
        stream << style
                      .arg("WL(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("WL(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("WL(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_DOB0")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "WL")
               << Qt::endl;

        stream << style
                      .arg("WH(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("WH(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("WH(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_DOB1")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "WH")
               << Qt::endl;
    }
    else if (szDataType == "s64" || szDataType == "u64")
    {
        stream << style
                      .arg("W0(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("W0(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("W0(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_QUD0")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "W0")
               << Qt::endl;

        stream << style
                      .arg("W1(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("W1(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("W1(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_QUD1")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "W1")
               << Qt::endl;

        stream << style
                      .arg("W2(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("W2(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("W2(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_QUD2")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "W2")
               << Qt::endl;

        stream << style
                      .arg("W3(" + szInitVal + ")", VALUE_WIDTH)
                      .arg("W3(" + szMinVal + ")", VALUE_WIDTH)
                      .arg("W3(" + szMaxVal + ")", VALUE_WIDTH)
                      .arg(szMode)
                      .arg(szRelate)
                      .arg(szSynCov)
                      .arg("B_QUD3")
                      .arg(szAddress.toUInt(), ADDR_WIDTH, 10, QLatin1Char('0'))
                      .arg(szParaName + "W3")
               << Qt::endl;
    }

    return szContent;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QXlsx::Document xlsx("paratbl.xlsx");

    QString     filePath = qApp->applicationDirPath() + "/paraattr.c";
    QFile       file(filePath);
    QTextStream filestream(&file);
    bool        isok = file.open(QIODevice::WriteOnly);

    if (isok)
    {
        filestream.setCodec("UTF-8");

        filestream << "// clang-format off" << Qt::endl;
        filestream << "{" << Qt::endl;

        for (quint16 row = 6; row < 700; row++)
        {
            QString szAddress = xlsx.cellAt(row, PARA_ADDRESS)->value().toString();

            if (szAddress.isEmpty())
            {
                qDebug() << row;
                break;
            }

            filestream << MakePara(szAddress,
                                   xlsx.cellAt(row, PARA_DATA_TYPE)->value().toString(),
                                   xlsx.cellAt(row, PARA_VAR_NAME)->value().toString(),
                                   xlsx.cellAt(row, PARA_ATTR_INTI_VAL)->value().toString(),
                                   xlsx.cellAt(row, PARA_ATTR_MIN_VAL)->value().toString(),
                                   xlsx.cellAt(row, PARA_ATTR_MAX_VAL)->value().toString(),
                                   xlsx.cellAt(row, PARA_SUBATTR_MODE)->value().toString(),
                                   xlsx.cellAt(row, PARA_SUBATTR_RELATE)->value().toString(),
                                   xlsx.cellAt(row, PARA_SUBATTR_SYNC_COVER)->value().toString(),
                                   xlsx.cellAt(row, PARA_SUBATTR_ACCESS)->value().toString());
        }

        filestream << "};" << Qt::endl;

        filestream << "// clang-format on" << Qt::endl;
    }

    return 0;

    //    MainWindow w;
    //    w.show();
    //    return a.exec();
}

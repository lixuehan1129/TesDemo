#pragma once
#pragma execution_character_set("utf-8")

#ifndef MY_HELPER_H
#define MY_HELPER_H

#include <QObject>
#include <QListWidget>
#include <QSettings>
#include <QWidget>
#include <QDesktopWidget>
#include <QFile>
#include <QFileIconProvider>
#include <QTextCodec>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QTime>
#include <QDateTime>
#include <QDate>
#include <QCoreApplication>
#include <QFileDialog>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QCursor>
#include <QTimer>
//#include <QSound>
#include <QApplication>
#include <QStyleFactory>
#include <QTextStream>
#include <QDebug>
#include <QWidget>
#include <QtWidgets/QTableWidget>
#include <QtCharts>
typedef uint16_t u16;
typedef uint8_t u8;

class Helper :public QObject
{
public:

	//设置全局为plastique样式
	static void SetPlastiqueStyle()
	{
		QApplication::setStyle(QStyleFactory::create("Plastique"));
		QApplication::setPalette(QApplication::style()->standardPalette());//替换应用程序的颜色组合
	}

	//显示信息框，仅确定按钮
	static void ShowMessageBoxInfo(QString info)
	{
		QMessageBox msg;
		msg.setStyleSheet("font:12pt '宋体'");
		msg.setWindowTitle(tr("提示"));
		msg.setText(info);
		msg.setIcon(QMessageBox::Information);
		msg.addButton(tr("确定"), QMessageBox::ActionRole);
		msg.exec();
	}

	//显示错误框，仅确定按钮
	static void ShowMessageBoxError(QString info)
	{
		QMessageBox msg;
		msg.setStyleSheet("font:12pt '宋体'");
		msg.setWindowTitle(tr("错误"));
		msg.setText(info);
		msg.setIcon(QMessageBox::Critical);
		msg.addButton(tr("确定"), QMessageBox::ActionRole);
		msg.exec();
	}

	//显示询问框，确定和取消按钮
	static int ShowMessageBoxQuesion(QString info)
	{
		QMessageBox msg;
		msg.setStyleSheet("font:12pt '宋体'");
		msg.setWindowTitle(tr("询问"));
		msg.setText(info);
		msg.setIcon(QMessageBox::Question);
		msg.addButton(tr("确定"), QMessageBox::ActionRole);
		msg.addButton(tr("取消"), QMessageBox::RejectRole);

		return msg.exec();
	}

	//16进制字符串转字节数组
	static QByteArray HexStrToByteArray(QString str)
	{
		QByteArray senddata;
		int hexdata, lowhexdata;
		int hexdatalen = 0;
		int len = str.length();
		senddata.resize(len / 2);
		char lstr, hstr;
		for (int i = 0; i < len; )
		{
			hstr = str[i].toLatin1();
			if (hstr == ' ')
			{
				i++;
				continue;
			}
			i++;
			if (i >= len)
				break;
			lstr = str[i].toLatin1();
			hexdata = ConvertHexChar(hstr);
			lowhexdata = ConvertHexChar(lstr);
			if ((hexdata == 16) || (lowhexdata == 16))
				break;
			else
				hexdata = hexdata * 16 + lowhexdata;
			i++;
			senddata[hexdatalen] = (char)hexdata;
			hexdatalen++;
		}
		senddata.resize(hexdatalen);
		return senddata;
	}

	static char ConvertHexChar(char ch)
	{
		if ((ch >= '0') && (ch <= '9'))
			return ch - 0x30;
		else if ((ch >= 'A') && (ch <= 'F'))
			return ch - 'A' + 10;
		else if ((ch >= 'a') && (ch <= 'f'))
			return ch - 'a' + 10;
		else return (-1);
	}

	//字节数组转16进制字符串
	static QString ByteArrayToHexStr(QByteArray data)
	{
		QString temp = "";
		QString hex = data.toHex();
		for (int i = 0; i < hex.length(); i = i + 2)
		{
			temp += hex.mid(i, 2) + " ";
		}
		return temp.trimmed().toUpper();
	}

	//16进制字符串转10进制
	static uchar StrHexToDecimal(QString strHex)
	{
		bool ok;
		return strHex.toInt(&ok, 16);
	}

	//10进制字符串转10进制
	static uchar StrDecimalToDecimal(QString strDecimal)
	{
		bool ok;
		return strDecimal.toInt(&ok, 10);
	}

	//16进制字符串转2进制字符串
	static QString StrHexToStrBin(QString strHex)
	{
		uchar decimal = StrHexToDecimal(strHex);
		QString bin = QString::number(decimal, 2);

		uchar len = bin.length();
		if (len < 8)
		{
			for (int i = 0; i < 8 - len; i++)
			{
				bin = "0" + bin;
			}
		}

		return bin;
	}

	//计算校验码
	static uchar GetCheckCode(uchar data[], uchar len)
	{
		uchar temp = 0;

		for (uchar i = 0; i < len; i++)
		{
			temp += data[i];
		}

		return temp % 256;
	}

	//延时
	static void Sleep(int sec)
	{
		QTime dieTime = QTime::currentTime().addMSecs(sec);
		while (QTime::currentTime() < dieTime)
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}

	//获取当前路径
	static QString GetCurrentPath()
	{
		return QString(QCoreApplication::applicationDirPath() + "/");
	}


	//获取当前日期时间星期
	static QString GetLongDateTime()
	{
		QDateTime time = QDateTime::currentDateTime();
		return time.toString("yyyy.MM.dd hh:mm:ss dddd");
	}

	//打开网址
	static void OpenUrlOrExe(QString url)
	{
		QDesktopServices::openUrl(QUrl(url));
	}

	//判断是否闰年
	static bool IsLeapYear(QString strDate)
	{
		int year = strDate.left(4).toInt();
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//查找表格中的相关项，并且只显示匹配项，其他项隐藏，部分对比查找
	static void findTextInTableWidget(QTableWidget *sourceTable, QString &findText) {

		//获取符合条件的cell索引
		int tempRowCount = sourceTable->rowCount();//获得行号
		QList <QTableWidgetItem *> item = sourceTable->findItems(findText, Qt::MatchContains);

		for (int i = 0; i < tempRowCount; i++)
		{
			sourceTable->setRowHidden(i, true);//隐藏所有行
		}

		if (!item.isEmpty())//*** 不为空**
		{
			for (int i = 0; i < item.count(); i++)
			{
				sourceTable->setRowHidden(item.at(i)->row(), false);//item.at(i).row()输出行号
			}
		}

	}

	/*ModBus 计算CRC码的步骤为：

	(1).预置16位寄存器为FFFFH。称此寄存器为CRC寄存器；
	(2).把第一个8位数据与CRC寄存器的低位相异或，把结果放于CRC寄存器；
	(3).把寄存器的内容右移一位(朝低位)，用0填补最高位，检查最低位；
	(4).如果最低位为0：重复第3步(再次移位)如果最低位为1：CRC寄存器与多项式A001（1010 0000 0000 0001）进行异或；
	(5).重复步骤3和4，直到右移8次，这样整个8位数据全部进行了处理；
	(6).重复步骤2到步骤5，进行下一个8位数据的处理；
	(7).最后得到的CRC寄存器即为CRC码。(CRC码 = CRC_L +CRC_H)

	*/

	/*****************crc校验**********************************************/

	//crc生成函数，并将crc存储在预先定于的数组中
	//调用方式crc16（指向数据的指针，需要校验的数据长度）
	static u16 crc16(u8 *ptr, u8 len)
	{

		u16 wcrc = 0XFFFF; //预置16位crc寄存器，初值全部为1

		u8 i = 0, j = 0; //定义计数

		for (i = 0; i < len; i++)
		{					//循环计算每个数据
			wcrc ^= *ptr++; //将八位数据与crc寄存器亦或.指针地址增加，指向下个数据

			for (j = 0; j < 8; j++)
			{ //循环计算数据的

				if (wcrc & 0X0001)
				{							   //判断右移出的是不是1，如果是1则与多项式进行异或。
					wcrc = wcrc >> 1 ^ 0XA001; //先将数据右移一位再与上面的多项式进行异或
				}
				else
				{				//如果不是1，则直接移出
					wcrc >>= 1; //直接移出
				}
			}
		}

		return wcrc; //高八位在前，低八位在后
	}

	static u8 *getCrcBuff(u8 *ptr, u8 len)
	{
		static u8 crcTemp[2];
		u16 wcrc = 0XFFFF; //预置16位crc寄存器，初值全部为1

		u8 i = 0, j = 0; //定义计数

		for (i = 0; i < len; i++)
		{					//循环计算每个数据
			wcrc ^= *ptr++; //将八位数据与crc寄存器亦或.指针地址增加，指向下个数据

			for (j = 0; j < 8; j++)
			{ //循环计算数据的

				if (wcrc & 0X0001)
				{							   //判断右移出的是不是1，如果是1则与多项式进行异或。
					wcrc = wcrc >> 1 ^ 0XA001; //先将数据右移一位再与上面的多项式进行异或
				}
				else
				{				//如果不是1，则直接移出
					wcrc >>= 1; //直接移出
				}
			}
		}
		crcTemp[0] = wcrc & 0x00FF;
		crcTemp[1] = (wcrc >> 8) & 0xFF;

		return crcTemp;
	}

	//
	static void changeTimeFromSecond(int *hour, int *minute, int *second) {

		*hour = *second / 3600;
		*minute = (*second - *hour * 3600) / 60;
		*second -= *hour * 3600 + *minute * 60;

	}

	//设置chartView的边缘宽度
	static void setLineChartMargins(QChart *chart, int margin) {
		QMargins m_Margin;
		m_Margin.setLeft(margin);
		m_Margin.setBottom(margin);
		m_Margin.setRight(margin);
		m_Margin.setTop(margin);

		chart->setMargins(m_Margin);
	}


};

class FormHelper :public QObject
{
public:

	//窗体居中显示
	static void FormInCenter(QWidget *frm)
	{
		QDesktopWidget desktop;
		int screenX = desktop.availableGeometry().width();
		int screenY = desktop.availableGeometry().height();
		int wndX = frm->width();
		int wndY = frm->height();
		QPoint movePoint(screenX / 2 - wndX / 2, screenY / 2 - wndY / 2);
		frm->move(movePoint);
	}

	//窗体没有最大化按钮
	static void FormNoMaxButton(QWidget *frm)
	{
		frm->setWindowFlags(Qt::WindowMinimizeButtonHint);
	}

	//窗体没有最大化和最小化按钮
	static void FormOnlyCloseButton(QWidget *frm)
	{
		frm->setWindowFlags(Qt::WindowMinMaxButtonsHint);
		frm->setWindowFlags(Qt::WindowCloseButtonHint);
	}

	//设置窗体透明度
	static void FormOpacity(QWidget *frm, double value)
	{
		frm->setWindowOpacity(value);
	}

	//窗体不能改变大小
	static void FormNotResize(QWidget *frm)
	{
		frm->setFixedSize(frm->width(), frm->height());
	}

	//窗体前端显示
	static void FormOnTop(QWidget *frm)
	{
		frm->setWindowFlags(Qt::WindowStaysOnTopHint);
	}

	//窗体后端显示
	static void FormOnBottom(QWidget *frm)
	{
		frm->setWindowFlags(Qt::WindowStaysOnBottomHint);
	}

	//获取桌面大小
	static QSize GetDesktopSize()
	{
		QDesktopWidget desktop;
		return QSize(desktop.availableGeometry().width(), desktop.availableGeometry().height());
	}

	//设置指定窗体为plastique样式
	static void SetPlastiqueStyle(QWidget *frm)
	{
		frm->setStyle(QStyleFactory::create("Plastique"));
	}

	static void SetCustomStyle(QWidget *frm)
	{
		QString myStyle1 = "QPushButton {";
		myStyle1.append("border: 2px solid #8f8f91; ");
		myStyle1.append("border-radius: 6px;");
		myStyle1.append("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #f6f7fa, stop: 1 #dadbde); ");
		myStyle1.append("min-width: 80px;}");
		myStyle1.append("QPushButton:pressed {");
		myStyle1.append("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dadbde, stop: 1 #f6f7fa); }");
		myStyle1.append("QPushButton:default  {");
		myStyle1.append("border-color: navy; /* 使得按钮显示变得更加突出 */}");

		QString myStyle2 = "QSlider::groove:horizontal {";
		myStyle2.append("border: 1px solid #999999;");
		myStyle2.append("height: 8px;");
		myStyle2.append("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);");
		myStyle2.append("margin: 2px 0;}");
		myStyle2.append("QSlider::handle:horizontal {");
		myStyle2.append("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);");
		myStyle2.append("border: 1px solid #5c5c5c;");
		myStyle2.append("width: 18px; margin: -2px 0;");
		myStyle2.append("border-radius: 3px;}");

		QString myStyle3 = "QGroupBox {";
		myStyle3.append("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,  stop: 0 #E0E0E0, stop: 1 #FFFFFF);");
		myStyle3.append("border: 2px solid gray;");
		myStyle3.append("border-radius: 5px;");
		myStyle3.append("margin-top: 1ex;}");
		myStyle3.append("QGroupBox::title {");
		myStyle3.append("subcontrol-origin: margin; ");
		myStyle3.append("subcontrol-position: top center;");
		myStyle3.append("padding: 0 3px;}");

		frm->setStyleSheet(myStyle1 + myStyle2 + myStyle3);
	}

	//改变样式
	static bool InstallStyle(QWidget* window, QString styleFile)
	{
		QFile file(styleFile);
		QString strStyleSheet;
		if (!file.open(QIODevice::WriteOnly))
		{
			strStyleSheet = "";
		}
		else
		{
			strStyleSheet = file.readAll();
		}
		file.close();

		if (strStyleSheet.isEmpty())
		{
			return false;
		}
		window->setStyleSheet(strStyleSheet);
		return true;
	}
};


class FileHelper : public QObject
{
public:

	//获取选择文件
	static QString GetFileName(QString filter)
	{
		return QFileDialog::getOpenFileName(NULL, tr("选择文件"), QCoreApplication::applicationDirPath(), filter);
	}

	//获取选择文件集合
	static QStringList GetFileNames(QString filter)
	{
		return QFileDialog::getOpenFileNames(NULL, tr("选择文件"), QCoreApplication::applicationDirPath(), filter);
	}

	//获取选择文件夹中的文件
	static QStringList GetFolderFileNames(QStringList filter)
	{
		QStringList fileList;
		QString strFolder = QFileDialog::getExistingDirectory();
		if (!strFolder.length() == 0)
		{
			QDir myFolder(strFolder);

			if (myFolder.exists())
			{
				fileList = myFolder.entryList(filter);
			}
		}
		return fileList;
	}

	//文件是否存在
	static bool FileExists(QString strFile)
	{
		QFile tempFile(strFile);
		if (tempFile.exists())
		{
			return true;
		}
		return false;
	}

	//文件夹是否存在
	static bool FolderExists(QString strFolder)
	{
		QDir tempFolder(strFolder);
		if (tempFolder.exists())
		{
			return true;
		}
		return false;
	}

	//移除文件
	static bool RemoveFile(QString strFile)
	{
		return QFile::remove(strFile);
	}

	//重命名文件
	static bool RenameFile(QString oldName, QString newName)
	{
		return QFile::rename(oldName, newName);
	}

	//获取文件目录
	static QString GetBasePath(QString strFullPath)
	{
		QFileInfo fileInfo(strFullPath);
		return fileInfo.path();
	}

	//获取拓展名
	static QString GetExtension(QString strFile)
	{
		QFileInfo fileInfo(strFile);
		return fileInfo.suffix();
	}

	//获取文件夹名称
	static QString GetFolderName(QString strFullPath)
	{
		QString strPath(strFullPath);
		strPath.replace('\\', '/');
		int pos = strPath.lastIndexOf('/');
		return strPath.mid(pos + 1);
	}

	//复制文件
	static bool CopyFile(QString sourceFile, QString targetFile)
	{
		if (FileExists(targetFile))
		{
			int ret = QMessageBox::information(NULL, tr("提示"), tr("文件已经存在，是否替换？"), QMessageBox::Yes | QMessageBox::No);
			if (ret != QMessageBox::Yes)
			{
				return false;
			}
		}
		return QFile::copy(sourceFile, targetFile);
	}

	//判断是否是文件
	static bool IsFile(QString strFile)
	{
		QFileInfo fileInfo(strFile);
		return fileInfo.isFile();
	}

	//判断是否是文件夹
	static bool IsFolder(QString strFolder)
	{
		QFileInfo fileInfo(strFolder);
		return fileInfo.isDir();
	}

	//追加文本
	static bool AppendText(QString strFile, QString strContent)
	{
		QFile inFile(strFile);
		if (!inFile.open(QIODevice::Append | QIODevice::Text))
		{
			QMessageBox::information(NULL, tr("提示"), tr("打开文件出错！"));
			return false;
		}
		QTextStream inStream(&inFile);
		inStream << strContent;
		inFile.close();
		return true;
	}

	//创建文件夹
	static bool CreateFolder(QString strFolder)
	{
		QDir dir;
		return dir.mkdir(strFolder);
	}

	//创建文件
	static bool CreateFile(QString strFile)
	{
		QFile file(strFile);
		if (!file.open(QIODevice::WriteOnly))
		{
			return false;
		}
		file.close();
		return true;
	}

	//读取文件内容
	static QString GetFileContent(QString strFile)
	{
		QFile file(strFile);
		QString txt;
		if (!file.open(QIODevice::WriteOnly))
		{
			txt = "";
		}
		else
		{
			txt = file.readAll();
		}
		file.close();
		return txt;
	}

};


#endif // MYHELPER_H
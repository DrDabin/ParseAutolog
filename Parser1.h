//---------------------------------------------------------------------------

#ifndef Parser1H
#define Parser1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <System.RegularExpressions.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Xml.XMLDoc.hpp>
#include <Xml.xmldom.hpp>
#include <Xml.XMLIntf.hpp>
#include <memory>    //std::auto_prt<>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TOpenDialog *ODialog;
	TSaveDialog *SDialog;
	TMainMenu *FMenu;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TPanel *Panel3;
	TStatusBar *StatusBar1;
	TLabel *Log;
	TEdit *Edit1;
	TButton *Button1;
	TRichEdit *RichEdit1;
	TCheckBox *CheckBox1;
	TMenuItem *N4;
	TMenuItem *SQLsystemfile1;
	TMenuItem *infotxt1;
	TXMLDocument *XMLDocument;
	void __fastcall OpenMMenu(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall SQLsystemfile1Click(TObject *Sender);
	void __fastcall infotxt1Click(TObject *Sender);
private:	// User declarations
	//Определяем Язык лога рус или англ.
	void __fastcall TForm1::OpredelenieTranslitRusEND();
	UnicodeString NameLog;//Путь до Лога.
	UnicodeString DirPatch; //Панка Windows
	TStringList *LogFile;// Загруженный Файл Log RSIT
	TStringList *LogParse;// Обработанный лог
	UnicodeString bitOS; // Содержит битность системы.
    bool syswow64;
	//Очищаем глобальные списки
	void __fastcall TForm1::CLEARStringList();
	//Определяем битность системы х86 х64
	void __fastcall TForm1::OpredelenieBitnoct();
	// Определяем диск на котором установлена ОС
	void __fastcall TForm1::OpredelenieDir();
	//Определяем Имя пользователя. Пока не знаю для чего)))
	void __fastcall TForm1::OpredelenieUser();
	// Дата лога.
	void __fastcall TForm1::OpredelenieData();
	//Определяем Версию Windows
	void __fastcall TForm1::OpredelenieOS();
	//Общий анализ лога RSIT
	void __fastcall TForm1::Analiz();
	//Поиск файла в таблице systemfile
    UnicodeString __fastcall TForm1::AnalizFileName(UnicodeString FileName);
	//Поиск службы/драйвера в таблице  "sysdrive"
	//Если есть то легал
	UnicodeString __fastcall TForm1::ParseServices(UnicodeString NameDrive);
	//Функция анализа секций драйвера /службы
	void __fastcall TForm1::AnalizSysDriveTest(int begin, int end);
    //Разделяем строку по символу ;
	//S3 FACEITService;FACEITService; C:\Program Files\FACEIT AC\faceitservice.exe [2020-11-12 18343264]
	void __fastcall TForm1::ParseStrokSysDrive(UnicodeString name, TStringList *Spisok);
    //Извлекаем имя файла. Нормализация полного имени файла
	// удаление "\\??\\" , замена %systemroot% и тд
	UnicodeString __fastcall TForm1::ParseFileNemeDrive(UnicodeString NameFileDrive);
	// Извлекаем и Анализируем лог Джека.
	int __fastcall TForm1::AnalizLogHJ(int begin);
	//Извлекаем список процессов из лога RSIT
	int __fastcall TForm1::AnalizListProcess(int begin);
	 //добавляет слешь символ перед слеш символом "\" что бы в яваскрипт входило по два слеша, один удалится.
	UnicodeString __fastcall TForm1::ObrabotkaPatchFile(UnicodeString put);
	// Функция поиска и проверка по пути из таблицы systemfile
	/* возвращающее значение
		0 - файла нет
		1 - файл есть и его путь совпадает
		2 - файл есть, но основной путь не совпадает, содержится в одно именной папке это толькуо для систем Х64
		3 - файл есть, но пути не совпадают. Скорее вирус.
	*/
	int __fastcall TForm1::LegalSysFile(UnicodeString NameFile);
	// Функция поиска и проверка по пути из таблицы namefile
	/*


    */
	int __fastcall TForm1::LegalNameFile(UnicodeString NameFile);
	// Функция простого поиска значения в БД
	//Аргументы 1 Что ищем, 2 - Таблица базы.
	// true есть значение, false нет значения
	bool __fastcall TForm1::SearchBD(UnicodeString Name, UnicodeString tablBase);
	// ======Папка назначенных заданий======
	int __fastcall TForm1::AnalizJOB(int begin);
	//====== Список файлов и папок, измененных за последние 3 месяца ======
	int __fastcall TForm1::AnalizListDir(int begin);
	//Создаю список не известных файлов.
	void __fastcall TForm1::CreateListNewFiles(UnicodeString Filename);
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

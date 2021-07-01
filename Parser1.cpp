//---------------------------------------------------------------------------

#include <vcl.h>
#include <System.RegularExpressions.hpp>
#include <memory>    //std::auto_prt<>
#include "sqlite\sqlite3.h"
#pragma comment(lib, "sqlite\sqlite3.lib")
#pragma hdrstop

#include "Parser1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OpenMMenu(TObject *Sender)
{
	if(ODialog->Execute())
	{
		CLEARStringList();
		LogFile->LoadFromFile(ODialog->FileName);
		//определяем имя открываемого файла
		NameLog = ExtractFileName(ODialog->FileName);
		NameLog = NameLog.SubString(1 , NameLog.Length()- 3); // определяем имя файла без расширения.
		NameLog = NameLog + "html";  // присваиваем к имени файла расширение  html.
		NameLog = ExtractFilePath(ODialog->FileName) + NameLog;
		OpredelenieTranslitRusEND();
	}
	else
	{
		String Message = "Откройте файл log.txt!";
		String Message2 = "Внимание. Ошибка!";
		Application->MessageBox(Message.w_str(), Message2.w_str(), MB_OK+MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++
void __fastcall TForm1::OpredelenieTranslitRusEND()
{

   UnicodeString proverka = LogFile->Strings[0];

   if(proverka.LowerCase().SubString(0, 3) == "log")
   {
      ShowMessage("En");
	  //HTMLrus->Visible = false;
	  //HTMLend->Visible = true; // включил Создать лог HTML англ
   }

   if(proverka.LowerCase().SubString(0, 3) == "лог")
   {
	   OpredelenieBitnoct();
	   OpredelenieDir();
	   OpredelenieUser();
	   OpredelenieData();
	   OpredelenieOS();
	   Analiz();
   }

}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	LogFile = new TStringList();
	LogParse = new TStringList();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	delete LogFile;
	delete LogParse;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __fastcall TForm1::CLEARStringList()
{
	LogFile->Clear();
	LogParse->Clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __fastcall TForm1::OpredelenieBitnoct()
{
	// 6 строка Содержит
	// X64

	bitOS = LogFile->Strings[5];

	if(LogFile->Strings[5].Trim().LowerCase() == "x86")
		syswow64 = false;
	else if(LogFile->Strings[5].Trim().LowerCase() == "x64")
		syswow64 = true;
	else
		syswow64 = false;
	StatusBar1->Panels->Items[1]->Text = bitOS;
}
//+++++++++++++++++++++++++++++++++++
void __fastcall TForm1::OpredelenieDir()
{
	//4 строка
	//Системный раздел C: размер 72 GB (32%) Свободно 229 GB
	UnicodeString proverka = LogFile->Strings[3];
	if(proverka.Pos("Системный раздел")!=-1)
	{
		proverka = proverka.SubString(proverka.Pos(":")-1,1);
		DirPatch = proverka.LowerCase() + ":\\windows\\";
		StatusBar1->Panels->Items[0]->Text = DirPatch;
	}

}
//+++++++++++++++++++++++++++++++++++
void __fastcall TForm1::OpredelenieUser()
{
	//Run by User at 2021-01-31 08:27:53
	UnicodeString proverka = LogFile->Strings[1];
	if(proverka.Pos("by") !=-1)
	{
		proverka = proverka.SubString(proverka.Pos("by")+2, proverka.Pos(" at ")-(proverka.Pos("by")+2)).Trim();
		//StatusBar1->Panels->Items[3]->Text = proverka;
	}
}
//++++++++++++++++++++++++++++++++++
void __fastcall TForm1::OpredelenieData()
{
	//Run by User at 2021-01-31 08:27:53
	UnicodeString proverka = LogFile->Strings[1];
	if(proverka.Pos(" at") !=-1)
	{
		proverka = proverka.SubString(proverka.Pos(" at ")+4, 10).Trim();
		StatusBar1->Panels->Items[3]->Text = proverka;
	}
}
//+++++++++++++++++++++++++
//Определяем Версию Windows
void __fastcall TForm1::OpredelenieOS()
{
	//Майкрософт Windows 10 Домашняя
	StatusBar1->Panels->Items[4]->Text = LogFile->Strings[2];
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
//Главная функция Общий анализ лога RSIT.
void __fastcall TForm1::Analiz()
{
	//Вставляем начальную заготовку.
	LogParse->LoadFromFile("Base\\javaBegin.txt", TEncoding::UTF8);

	for(int i =0; i < LogFile->Count;)
	{
		//Извлекаем лог Джека
		//Начиная с Running processes:
		if(LogFile->Strings[i].Pos("processes:") != 0)
		{
				LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i] +"<br>") ;
				i = i+1;
				i = AnalizLogHJ(i+1)+1;

		}

		//Извлекаем список процессов из лога RSIT
		if(LogFile->Strings[i].Pos("Список процессов")!=0)
		{
			//Добавляю в лог сразу две строки, что бы в  функцию не пошла первая пустая строка.
			//так как пустая строка делает выход из функции.
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i] +"<br>") ;
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i+1] +"<br>") ;
			i = AnalizListProcess(i+2);

		}

		//======Папка назначенных заданий======
		if(LogFile->Strings[i].Pos("Папка назначенных заданий") !=0)
			i = AnalizJOB(i);

		//====== Список файлов и папок, созданных за последние 3 месяца ======
		//====== Список файлов и папок, измененных за последние 3 месяца ======
		if(LogFile->Strings[i].Pos("Список файлов и папок,") !=0)
			i = AnalizListDir(i);


		//Всегда последней.
		if(LogFile->Strings[i].Pos("Список драйверов") !=0)
		{
			AnalizSysDriveTest(i,LogFile->Count);
			//Выхожу из цикла, так как дастигнут конец файла
			//секции Драйвера и службы.
			break;
		}

		LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i] +"<br>") ;
		++i;
	}
	std::auto_ptr<TStringList>FileSpisokProwerki(new TStringList(NULL));
	FileSpisokProwerki->LoadFromFile("Base\\javaEnd.txt",TEncoding::UTF8);
	LogParse->AddStrings(FileSpisokProwerki.get());
	LogParse->SaveToFile("Log.html", TEncoding::UTF8);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __fastcall TForm1::AnalizListDir(int begin)
{
	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[begin]) ;
	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[begin+1]);
	LogParse->Add("<TABLE cellSpacing=1 cellPadding=2 width=\"100%\" bgColor=#ffffff border=0>");
	LogParse->Add("<TR BgColor=\"#0000FF\"><TD align=center><b><font color=White>Дата<TD align=center><b><font color=White>Атрибут<TD align=center><b><font color=White>Файл");
	begin +=2;
	for(begin; begin < LogFile->Count; begin++)
	{
		//Проверяем не пустая ли строка.
		if(LogFile->Strings[begin] =="" || LogFile->Strings[begin] ==" ")
		{
			LogParse->Add("</TABLE>");
			break;
		}

		UnicodeString data = LogFile->Strings[begin].SubString(1, LogFile->Strings[begin].Pos("---")-1);
		UnicodeString atrib = LogFile->Strings[begin].SubString( LogFile->Strings[begin].Pos(" ---"), (LogFile->Strings[begin].Pos("--- ")+3)-LogFile->Strings[begin].Pos(" ---"));
		UnicodeString file = LogFile->Strings[begin].SubString(LogFile->Strings[begin].Pos("--- ")+4, LogFile->Strings[begin].Length());
		// Если в атрибуте есть символ "D" || "d" то это директория, иначе файл.
		if(atrib.LowerCase().Pos("d")==0)
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + data +"<TD>" + atrib  +"<TD>" + AnalizFileName(ParseFileNemeDrive(file)) + "<TR>");
		else
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + data +"<TD>" + atrib  +"<TD>" + file + "<TR>");
	}
   return begin;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __fastcall TForm1::AnalizJOB(int begin)
{
	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[begin] +"<br>") ;
	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[begin+1] +"<br>");
	LogParse->Add("<TABLE cellSpacing=1 cellPadding=2 width=\"100%\" bgColor=#ffffff border=0>");
	LogParse->Add("<TR BgColor=\"#0000FF\"><TD align=center><b><font color=White>JOB<TD align=center><b><font color=White>Файл<TD align=center><b><font color=White>Аргумент");
	begin +=2;
	for(begin; begin < LogFile->Count; begin++)
	{
		//Проверяем не пустая ли строка.
		if(LogFile->Strings[begin] =="" || LogFile->Strings[begin] ==" ")
		{
			LogParse->Add("</TABLE>");
			break;
		}

		UnicodeString JOB = LogFile->Strings[begin].SubString(1,LogFile->Strings[begin].Pos(" - "));
		UnicodeString stroka =LogFile->Strings[begin].Delete(1,LogFile->Strings[begin].Pos(" - ")+2).Trim();
		UnicodeString file;
		if(stroka.SubString(1,1) =="\"")
		{
			file = stroka.Delete(1,1);
			file = file.SubString(1,file.Pos("\"")-1);
			stroka = stroka.Delete(1, stroka.Pos("\""));

		}

		if(stroka.Pos(".exe")!=0)
		{
			file = stroka.SubString(1,stroka.Pos(".exe")+3);
			stroka = stroka.Delete(1, stroka.Pos(".exe")+3);
		}


		LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + JOB +"<TD>" + AnalizFileName(ParseFileNemeDrive(file))  +"<TD>" + stroka + "<TR>&nbsp;");
		//=


	}
   return begin;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Обрабатываем список процессов из лога RSIT
int __fastcall TForm1::AnalizListProcess(int begin)
{
	for(begin; begin < LogFile->Count; begin++)
	{
		//Проверяем не пустая ли строка.
		if(LogFile->Strings[begin] =="" || LogFile->Strings[begin] ==" ")
			break;
		//заносим в лог
		LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + AnalizFileName(ParseFileNemeDrive(LogFile->Strings[begin])) +"<br>") ;

	}

	return begin;
}

//++++++++++++++++++++++++++++++++++++++
int __fastcall TForm1::AnalizLogHJ(int begin)
{
	int i = begin;
	for(i; i <LogFile->Count; i++)
	{
		if(LogFile->Strings[i].Pos("End of file") !=0)
		{
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i] +"<br>") ;
			break;
		}
		else
		{
			if(LogFile->Strings[i].SubString(2,1) ==":")
			{
				LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + AnalizFileName(ParseFileNemeDrive(LogFile->Strings[i])) +"<br>") ;

			}
			else
                LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[i] +"<br>") ;
        }

	}
	return i;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
 //добавляет слешь символ перед слеш символом "\" что бы в яваскрипт входило по два слеша, один удалится.
UnicodeString __fastcall TForm1::ObrabotkaPatchFile(UnicodeString put)
{
   UnicodeString temp="";
  for(int i = 1; i <= put.Length(); i++){
	  // Если символ "\" вставляем ещё такойже символ.
	 if(put.SubString(i,1)=="\\")
	   temp += "\\";
	   temp += put.SubString(i,1);
  }
	return temp;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Функция анализа секций драйвера /службы
void __fastcall TForm1::AnalizSysDriveTest(int begin, int end)
{
	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[begin] +"<br>") ;
	LogParse->Add("<TABLE cellSpacing=1 cellPadding=2 width=\"100%\" bgColor=#ffffff border=0>");
	LogParse->Add("<TR BgColor=\"#0000FF\"><TD align=center><b><font color=White>СТ<TD align=center><b><font color=White>Драйвер<TD align=center><b><font color=White>Описание<TD align=center><b><font color=White>Файл<TD align=center><b><font color=White>ServiceDll");

	for(int q = begin+2; q < end; q++)
	{
		if(LogFile->Strings[q] !="")
		{
			//если конец файла
			if(LogFile->Strings[q].LowerCase().Pos("---eof---") !=0)
				LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[q] +"<br>");
			//Если начало Секции "Список служб"
			if(LogFile->Strings[q].Pos("Список служб") !=0)
			{
				//Строку заворачиваем просто в тег
				LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[q] +"<br>") ;
                // заносим в лог создание таблицы.
				LogParse->Add("<TABLE width=\"100%\" bgColor=#ffffff>");
				LogParse->Add("<TR BgColor=\"#0000FF\"><TD align=center><b><font color=White>СТ<TD align=center><b><font color=White>Служба<TD align=center><b><font color=White>Описание<TD align=center><b><font color=White>Файл<TD align=center><b><font color=White>ServiceDll");

				q = q +1;
			}
			else
			{
				std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );

				ParseStrokSysDrive(LogFile->Strings[q], Spisok.get());
				
				if(Spisok->Count ==3)
					LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + Spisok->Strings[0].SubString(1,2) + "<TD>" + ParseServices(Spisok->Strings[0].SubString(4, Spisok->Strings[0].Length())) + "<TD>" + Spisok->Strings[1]  + "<TD>" + AnalizFileName(ParseFileNemeDrive(Spisok->Strings[2])) + "<TD><TR>&nbsp;"); // Берём имя службы в первой строке списка(возможно нужна корректировка)

				
				if( Spisok->Count ==4)
				{
					UnicodeString name3 = Spisok->Strings[3].Trim().LowerCase();
					//Удаляю в начале строки "ServiceDll"=
					//что бы путь был норм
					if(name3.Pos("\"servicedll\"") !=0)
					{
						name3 = name3.Delete(1,name3.Pos("="));



						/*if(name3.Pos("%systemroot%")!=0)
						{
							name3 = name3.Delete(name3.Pos("%systemroot%"),13);
							name3 = DirPatch.Trim() + name3.Trim();
						}*/

						UnicodeString name2 = Spisok->Strings[2].Trim().LowerCase();

						if(name2.Pos(" -k ") !=0)
							name2 = name2.SubString(1,name2.Pos(" -k "));

						/*if(name2.Pos("%systemroot%")!=0)
						{
							name2 = name2.Delete(name2.Pos("%systemroot%"),13);
							name2 = DirPatch.Trim() + name2.Trim();
						}*/
						LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + Spisok->Strings[0].SubString(1,2) + "<TD>" + ParseServices(Spisok->Strings[0].SubString(4, Spisok->Strings[0].Length())) + "<TD>" + Spisok->Strings[1]  + "<TD>" + AnalizFileName(ParseFileNemeDrive(name2)) + "<br><font size=-1>" + Spisok->Strings[2].Trim().LowerCase() +"</font><TD>" +  AnalizFileName(ParseFileNemeDrive(name3)) +"<TR>&nbsp;"); // Берём имя службы в первой строке списка(возможно нужна корректировка)

					}
					else
					{
						LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + Spisok->Strings[0].SubString(1,2) + "<TD>" + ParseServices(Spisok->Strings[0].SubString(4, Spisok->Strings[0].Length())) + "<TD>" + Spisok->Strings[1]  + "<TD>" + Spisok->Strings[2] +"</font><TD>" +  AnalizFileName(ParseFileNemeDrive(Spisok->Strings[3])) +"<TR>&nbsp;"); // Берём имя службы в первой строке списка(возможно нужна корректировка)
                    }




				 }
				//delete Spisok;	
				//if( Spisok->Count ==5)
				//	LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + Spisok->Strings[0].SubString(1,2) + "<TD>" + ParseServices(Spisok->Strings[0].SubString(4, Spisok->Strings[0].Length())) + "<TD>" + Spisok->Strings[1]  + "<TD nowrap>" + Spisok->Strings[2] + "<TD>" +  Spisok->Strings[4] +"<TR>&nbsp;"); // Берём имя службы в первой строке списка(возможно нужна корректировка)
			}
		}
		else
		{
			LogParse->Add("</TABLE>");
			LogParse->Add("<TR bgColor=\"#ffc06d\"><TD>" + LogFile->Strings[q] +"<br>") ;
		}
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Разделяем строку по символу ;
//S3 FACEITService;FACEITService; C:\Program Files\FACEIT AC\faceitservice.exe [2020-11-12 18343264]
void __fastcall TForm1::ParseStrokSysDrive(UnicodeString name, TStringList *Spisok)
{
	for (int i = 1; i < name.Length(); i++) 
	{
		if (name.Pos(";")!=0)
		{
		   if(name[i] == ';')
		   {
			  Spisok->Add(name.SubString(1,i-1).Trim());
			   name.Delete(1,i);
			   i=1;
		   }
		}
		else
		{
		   Spisok->Add(name.Trim());
		   break;
		}
	}       
	
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Извлекаем имя файла. Нормализация полного имени файла
// удаление "\\??\\" , замена %systemroot% и тд
UnicodeString __fastcall TForm1::ParseFileNemeDrive(UnicodeString NameFileDrive)
{
	UnicodeString file = NameFileDrive.LowerCase().Trim();
	if(file.Pos("\\??\\") !=0)
		file = file.Delete(1,4);

	if(file.Pos("[") !=0)
		file = file.Delete(file.Pos("["),file.Length()-(file.Pos("[")-1));

	if(file.SubString(1,1) == "\"")
	{
		file = file.Delete(1,1);
		file = file.SubString(1, file.Pos("\"")-1);
	}

	if(file.Pos(" -k ") !=0)
		file = file.SubString(1,file.Pos(" -k "));

	if(file.Pos("/") !=0)
		file = file.SubString(1, file.Pos("/")-1);
	if(file.Pos("\"") !=0)
		file = file.SubString(1, file.Pos("\"")-1);

	if(file.Pos("%systemroot%")!=0)
	{
		file = file.Delete(file.Pos("%systemroot%"),13);
		file = DirPatch.Trim() + file;
	}
	if(file.Pos("%windir%")!=0)
	{
		file = file.Delete(file.Pos("%windir%"),9);
		file = DirPatch.Trim() + file;
	}
	return file.Trim();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//Поиск файла в таблице systemfile
UnicodeString __fastcall TForm1::AnalizFileName(UnicodeString FileName)
{
	int rez;
	UnicodeString Rezult = FileName.Trim();
	if(FileName !="")
	{
		rez = LegalSysFile(FileName);

		if(rez == 0)
			rez = LegalNameFile(FileName);

		// для сбора, если файла нет в базе.
		if(rez ==0 && CheckBox1->Checked==true)
		{
			CreateListNewFiles(FileName.Trim());
        }

		switch (rez)
		{
			//Файла нет в БД таблицы systemfile
			case 0:
				//Rezult = FileName;
				FileName = ObrabotkaPatchFile(FileName);
				Rezult += "<br><font size=-2>Скрипт: <a href='Javascript:add_scr_k(\"" + FileName
					+ "\")'>Kарантин</a>, <a href='Javascript:add_scr_d(\""
					+  FileName
					+ "\")'>Удалить</a>, <a href='Javascript:add_scr_db(\""
					+ FileName
					+ "\")'>Удалить через BC</a></font>";

				break;
			//Файла есть в БД таблицы systemfile  и путь совпадает. Легальный файл
			case 1:
				Rezult =  "<span class=\"legal\">" + FileName + UnicodeString("</span>");
				FileName = ObrabotkaPatchFile(FileName);
				Rezult +=  "<br><font size=-2>Скрипт: <a href='Javascript:add_scr_k(\"" + FileName
					+ "\")'>Kарантин</a>, <a href='Javascript:add_scr_d(\""
					+  FileName
					+ "\")'>Удалить</a>, <a href='Javascript:add_scr_db(\""
					+ FileName
					+ "\")'>Удалить через BC</a></font>";
				break;
			//Файла есть в БД таблицы systemfile  и путь не совпадает. Вирусный файл
			case 3:
				Rezult =  "<span class=\"virus\">" + FileName + UnicodeString("</span>");
                FileName = ObrabotkaPatchFile(FileName);
				Rezult +=  "<br><font size=-2>Не совпадает путь из БД.</font><br><font size=-2>Скрипт: <a href='Javascript:add_scr_k(\"" + FileName
					+ "\")'>Kарантин</a>, <a href='Javascript:add_scr_d(\""
					+  FileName
					+ "\")'>Удалить</a>, <a href='Javascript:add_scr_db(\""
					+ FileName
					+ "\")'>Удалить через BC</a></font>";
				break;
		}			
	}
	return Rezult;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Поиск службы/драйвера в таблице  "sysdrive"
//Если есть то легал
UnicodeString __fastcall TForm1::ParseServices(UnicodeString NameDrive)
{
	//Перевожу в нижний регист так как все службы в базе в нижнем регистре.
	NameDrive = NameDrive.LowerCase();
	if(SearchBD(NameDrive,"sysdrive"))
	{

		NameDrive = "<span class=\"legal\">" + NameDrive + UnicodeString("</span><br><font size=-2>Служба: <a href='Javascript:add_scr_s1(\"") + NameDrive
		+ UnicodeString("\")'>Стоп</a>,  <a href='Javascript:add_scr_s2(\"")
		+ NameDrive
		+ UnicodeString("\")'>Удалить</a>,  <a href='Javascript:add_scr_s3(\"")
		+ NameDrive
		+ UnicodeString("\")'>Отключить</a>, <a href='Javascript:add_scr_s4(\"")
		+ NameDrive
		+ UnicodeString("\")'>Удалить через BC</a>");
	}
	else
		NameDrive = NameDrive + UnicodeString("<br><font size=-2>Служба: <a href='Javascript:add_scr_s1(\"")
			+ NameDrive
			+ UnicodeString("\")'>Стоп</a>,  <a href='Javascript:add_scr_s2(\"")
			+ NameDrive
			+ UnicodeString("\")'>Удалить</a>,  <a href='Javascript:add_scr_s3(\"")
			+ NameDrive
			+ UnicodeString("\")'>Отключить</a>, <a href='Javascript:add_scr_s4(\"")
			+ NameDrive
			+ UnicodeString("\")'>Удалить через BC</a>");


	return NameDrive;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __fastcall TForm1::LegalSysFile(UnicodeString NameFile)
{
	NameFile = NameFile.LowerCase();
	int ret=0;
	sqlite3 *db;
	sqlite3_stmt * pStmt;
	UnicodeString name = ExtractFileName(NameFile).Trim();

	AnsiString str = "SELECT patchfile,patch,iswow64 From systemfile WHERE namefile=\"" + name + "\"";
	// открытие (создание) БД
	if (sqlite3_open("Base\\base.db", &db))
	{
		sqlite3_close(db);
	}
	if (sqlite3_prepare(db, str.c_str(), -1, &pStmt, NULL))
	{
		sqlite3_finalize(pStmt);
		sqlite3_close(db);
	}
	if(sqlite3_step(pStmt) == SQLITE_ROW)
	{
		// для остальных запросов функция
		//sqlite3_step вернет SQLITE_DONE

		UnicodeString PatchFile = UTF8ToUnicodeString((char *) sqlite3_column_text(pStmt, 0));
		UnicodeString Patch =  UTF8ToUnicodeString((char *) sqlite3_column_text(pStmt, 1));
		int iswow =  StrToInt(UnicodeString((char *) sqlite3_column_text(pStmt, 2)));
		sqlite3_finalize(pStmt);
		sqlite3_close(db);

		std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
		Spisok->Delimiter = ';';
		Spisok->StrictDelimiter = true;
		Spisok->DelimitedText = PatchFile;

		//Если в PatchFile не сколько путей разделенных через ;
		if(Spisok->Count >1)
		{
			if(syswow64 == false)
			{
				std::auto_ptr<TRegEx> re(new TRegEx);
				if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[0] + Patch))
					ret =1;
				else
					ret = 3;

			}
			if(syswow64)
			{
				std::auto_ptr<TRegEx> re(new TRegEx);

				if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[iswow] + Patch))
				{
				ret =1;
				}
				else
				{
					if(iswow == 2)
						ret = 3;
					else
					{
						if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[2] + Patch))
							ret=2;
						else
							ret = 3;
					}
				}
			}
		}
		//Если в PatchFile один путь
		else
		{
			std::auto_ptr<TRegEx> re(new TRegEx);
			if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[0] + Patch))
				ret =1;
			else
				ret = 3;
        }

	}
	else
	{
		sqlite3_finalize(pStmt);
		sqlite3_close(db);
	}

	return ret;
}

//---------------------------------------------------------------------------
 //таблица namefile
 int __fastcall TForm1::LegalNameFile(UnicodeString NameFile)
{
	NameFile = NameFile.LowerCase();
	int ret=0;
	sqlite3 *db;
	sqlite3_stmt * pStmt;
	UnicodeString name = ExtractFileName(NameFile).Trim();

	AnsiString str = "SELECT patchfile,iswow64 From namefile WHERE namefile=\"" + name + "\"";
	// открытие (создание) БД
	if (sqlite3_open("Base\\base.db", &db))
	{
		sqlite3_close(db);
	}
	if (sqlite3_prepare(db, str.c_str(), -1, &pStmt, NULL))
	{
		sqlite3_finalize(pStmt);
		sqlite3_close(db);
	}
	if(sqlite3_step(pStmt) == SQLITE_ROW)
	{
		// для остальных запросов функция
		//sqlite3_step вернет SQLITE_DONE

		UnicodeString PatchFile = UTF8ToUnicodeString((char *) sqlite3_column_text(pStmt, 0));
		int iswow =  StrToInt(UnicodeString((char *) sqlite3_column_text(pStmt, 1)));
		sqlite3_finalize(pStmt);
		sqlite3_close(db);

		std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
		Spisok->Delimiter = ';';
		Spisok->StrictDelimiter = true;
		Spisok->DelimitedText = PatchFile;

		//Если в PatchFile не сколько путей разделенных через ;
		if(Spisok->Count >1)
		{
			if(syswow64 == false)
			{
				std::auto_ptr<TRegEx> re(new TRegEx);
				if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[0]))
					ret =1;
				else
					ret = 3;

			}
			if(syswow64)
			{
				std::auto_ptr<TRegEx> re(new TRegEx);

				if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[iswow]))
				{
				ret =1;
				}
				else
				{
					if(iswow == 2)
						ret = 3;
					else
					{
						if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[2]))
							ret=2;
						else
							ret = 3;
					}
				}
			}
		}
		//Если в PatchFile один путь
		else
		{
			std::auto_ptr<TRegEx> re(new TRegEx);
			if(re->IsMatch(ExtractFilePath(NameFile), Spisok->Strings[0]))
				ret =1;
			else
				ret = 3;
        }

	}
	else
	{
		sqlite3_finalize(pStmt);
		sqlite3_close(db);
	}

	return ret;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool __fastcall TForm1::SearchBD(UnicodeString Name, UnicodeString tablBase)
{
	Name = Name.LowerCase();
	int ret=false;
	sqlite3 *db;
	sqlite3_stmt * pStmt;
    //инструкция которая выводит данные начиная с patchfile без md5
	AnsiString str;// = "SELECT patchfile, namefile From systemfile WHERE md5=\"" + md5 + "\"";

	str = "SELECT name From " + tablBase + " WHERE name=\"" + Name + "\";";
	// открытие (создание) БД
	if (sqlite3_open("Base\\base.db", &db))
	{
		sqlite3_close(db);
	}
	//
	if (sqlite3_prepare(db, str.c_str(), -1, &pStmt, NULL))
	{
		sqlite3_finalize(pStmt);
		sqlite3_close(db);
	}
	// Если значения есть (SQLITE_ROW), то передаем tru. Иначе false.
	if(sqlite3_step(pStmt) == SQLITE_ROW)
		ret = true;

	sqlite3_finalize(pStmt);
	sqlite3_close(db);

	return ret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
	ParseStrokSysDrive(Edit1->Text,Spisok.get());
	RichEdit1->Text= Spisok->Text; 
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateListNewFiles(UnicodeString Filename)
{
	std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
	std::auto_ptr<TStringList> namefile (new TStringList(NULL) );
	Spisok->LoadFromFile("Base\\systemfile.txt");
	namefile->LoadFromFile("Base\\namefile.txt");
    Spisok->Duplicates =System::Types::dupIgnore;
	Spisok->Sorted = true;
    namefile->Sorted = true;
	namefile->Duplicates = System::Types::dupIgnore;
	if(Filename.LowerCase().Pos("\\windows\\") !=0 || Filename.LowerCase().Pos("\\system")!=0)
		Spisok->Add(ExtractFileName(Filename) +  "=" +  Filename);
		//Spisok->Add("INSERT INTO systemfile (namefile, patchfile, patch, vendor, iswow64) VALUES ('" + ExtractFileName(Filename) + "', '"+ Filename+"', '$', 0, '');");

	else
		namefile->Add(ExtractFileName(Filename) +  "=" +  Filename);



	Spisok->SaveToFile("Base\\systemfile.txt");

	namefile->SaveToFile("Base\\namefile.txt");
}
void __fastcall TForm1::SQLsystemfile1Click(TObject *Sender)
{
	std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
	std::auto_ptr<TStringList> Spisok2 (new TStringList(NULL) );
	Spisok->LoadFromFile("Base\\systemfile.txt");

	for(int i = 0; i<Spisok->Count;)
	{
		 UnicodeString stroka = Spisok->Strings[i];
         Spisok->Delete(i);
		 UnicodeString name = stroka.SubString(1,stroka.Pos("=")-1);

		 if(Spisok->IndexOfName(name)!=-1)
		 {
		   Spisok2->Add(Spisok->Strings[Spisok->IndexOfName(name)]) ;
		   Spisok->Delete(Spisok->IndexOfName(name)) ;
		 }
		 else
			i++;
	}
	Spisok2->SaveToFile("Base\\systemSQL.txt");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::infotxt1Click(TObject *Sender)
{
    if(ODialog->Execute())
	{
		std::auto_ptr<TStringList> Spisok (new TStringList(NULL) );
		Spisok->LoadFromFile(ODialog->FileName);
		XMLDocument->Active = true;
		XMLDocument->LoadFromFile("Base\\SCUpdate.xml");
		IXMLNode *xmlRoot; // для лога AVZ .xml
		_di_IXMLNode NodeList;
		xmlRoot = XMLDocument->ChildNodes->Nodes[1];
        TMatch match;
		std::auto_ptr<TRegEx> re(new TRegEx);


        for(int i = 0; i < xmlRoot->ChildNodes->Count; i++)
		{
			if(xmlRoot->ChildNodes->Nodes[i]->NodeName == "ProxyAndVPNs")
			{
				NodeList = xmlRoot->ChildNodes->Nodes[i];
				UnicodeString nameRGX= "";
                for(int j = 0; j < NodeList->ChildNodes->Count; j++)
				{
					if(NodeList->ChildNodes->Get(j)->AttributeNodes->IndexOf("Pattern")>=0)
					{
						nameRGX = NodeList->ChildNodes->Get(j)->GetAttribute("Name");
						TMatch match;

						std::auto_ptr<TRegEx> re(new TRegEx);
						match = re->Match(Spisok->Text,nameRGX,TRegExOptions() << roIgnoreCase << roMultiLine);

						if(match.Length !=0)
						{
							RichEdit1->Lines->Add(match.Value + " = Внимание! Это приложение может отображать рекламу на посещаемых страницах.");
						}

					}
				}

			}

        }

	}
	else
	{
		String Message = "Откройте файл info.txt!";
		String Message2 = "Внимание. Ошибка!";
		Application->MessageBox(Message.w_str(), Message2.w_str(), MB_OK+MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------


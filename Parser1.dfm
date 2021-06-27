object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'ParserAutologer'
  ClientHeight = 160
  ClientWidth = 971
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = FMenu
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 116
    Width = 971
    Height = 44
    Align = alBottom
    TabOrder = 0
    object Log: TLabel
      Left = 1
      Top = 6
      Width = 3
      Height = 13
    end
    object StatusBar1: TStatusBar
      Left = 1
      Top = 24
      Width = 969
      Height = 19
      Panels = <
        item
          Width = 81
        end
        item
          Width = 30
        end
        item
          Width = 50
        end
        item
          Width = 65
        end
        item
          Width = 50
        end>
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 971
    Height = 68
    Align = alClient
    TabOrder = 1
    object RichEdit1: TRichEdit
      Left = 1
      Top = 1
      Width = 969
      Height = 66
      Align = alClient
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Lines.Strings = (
        'RichEdit1')
      ParentFont = False
      ScrollBars = ssBoth
      TabOrder = 0
      Zoom = 100
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 68
    Width = 971
    Height = 48
    Align = alBottom
    TabOrder = 2
    object Edit1: TEdit
      Left = 1
      Top = 6
      Width = 276
      Height = 21
      TabOrder = 0
      Text = 'Edit1'
    end
    object Button1: TButton
      Left = 340
      Top = 5
      Width = 75
      Height = 25
      Caption = 'Button1'
      TabOrder = 1
      OnClick = Button1Click
    end
    object CheckBox1: TCheckBox
      Left = 840
      Top = 25
      Width = 125
      Height = 17
      Caption = #1057#1073#1086#1088' '#1085#1077' '#1080#1079#1074#1077#1089#1090#1085#1099#1093
      TabOrder = 2
    end
  end
  object ODialog: TOpenDialog
    Left = 760
    Top = 12
  end
  object SDialog: TSaveDialog
    Left = 888
    Top = 16
  end
  object FMenu: TMainMenu
    Left = 32
    Top = 12
    object N1: TMenuItem
      Caption = #1060#1072#1081#1083
      object N2: TMenuItem
        Caption = #1054#1090#1082#1088#1099#1090#1100
        OnClick = OpenMMenu
      end
      object infotxt1: TMenuItem
        Caption = #1055#1072#1088#1089#1080#1085#1075' info.txt'
        OnClick = infotxt1Click
      end
      object N3: TMenuItem
        Caption = #1042#1099#1093#1086#1076
      end
    end
    object N4: TMenuItem
      Caption = #1057#1077#1088#1074#1080#1089
      object SQLsystemfile1: TMenuItem
        Caption = #1055#1077#1088#1077#1074#1077#1089#1090#1080' '#1089' '#1087#1080#1089#1082#1072' '#1074' SQL systemfile'
        OnClick = SQLsystemfile1Click
      end
    end
  end
  object XMLDocument: TXMLDocument
    Left = 680
    Top = 12
  end
end

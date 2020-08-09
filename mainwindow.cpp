//Open Braille Printer (C) 2017 adhsoft0@gmail.com Todos los derechos reservados. 

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QString>

#define   VALORCENTRO               10
#define   MAX_CARACTERES            600
#define   COMANDO_FUNCION           "ÿ"     // 0xFF
#define   COMANDO_MOVER             "þ"     // 0xFE
#define   FUNCION_EXPULSAR          "ý"     // 0xFD
#define   FUNCION_PREGUNTAR_HOJA    "û"     // 0xFB
#define   NUEVO                     0
#define   ABRIR                     1

/******************************************* CONSTRUCTOR ***************************************************/
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    estado = Editando;
    estado_puerto = Desconectado;
    p_puerto = NULL;
    timer_label = NULL;
    flag_imp = 0;
    boton = NUEVO;

    setWindowTitle( "Maquina Braille" );
    ui->label_EstadoPort->setStyleSheet( "font-weight: bold; color: black; " );
    ui->label_EstadoPort->setText( "Desconectado" );

    ui->EditorDeTexto->setFocus();
    ui->EditorDeTexto->selectAll();
    ui->label_Mensaje->setText("");
    ui->lineEdit_Path->setText("");
    ui->groupBox_Papel->setDisabled( true );

    connect( ui->horizontalSlider, SIGNAL( valueChanged( int ) ), this, SLOT( actualizar_texto_desplazamiento() ) );
    connect( ui->comboBox_Port   , SIGNAL( activated( QString )), this, SLOT( Conectar() ) );
    connect( ui->pushButton_Conectar   , SIGNAL( clicked(bool) ), this, SLOT( Conectar() ) );
    connect( ui->pushButton_Actualizar , SIGNAL( clicked(bool) ), this, SLOT( actualizar_lista_puertos() ) );
    connect( ui->pushButton_Nuevo      , SIGNAL( clicked(bool) ), this, SLOT( ArchivoNuevo()   ) );
    connect( ui->pushButton_Abrir      , SIGNAL( clicked(bool) ), this, SLOT( AbrirArchivo()   ) );
    connect( ui->pushButton_Guardar    , SIGNAL( clicked(bool) ), this, SLOT( GuardarArchivo() ) );
    connect( ui->pushButton_Ayuda      , SIGNAL( clicked(bool) ), this, SLOT( MostrarAyuda()   ) );
    connect( ui->pushButton_Imprimir   , SIGNAL( clicked(bool) ), this, SLOT( ImprimirTexto()  ) );
    connect( ui->pushButton_Salir      , SIGNAL( clicked(bool) ), this, SLOT( Salir()          ) );
    connect( ui->pushButton_mover      , SIGNAL( clicked(bool) ), this, SLOT( AjustarMotor()   ) );
    connect( ui->pushButton_Retirar    , SIGNAL( clicked(bool) ), this, SLOT( RetirarPapel()   ) );

    actualizar_lista_puertos();
    actualizar_texto_desplazamiento();

    timer = new QTimer();
    connect( timer, SIGNAL( timeout() ), this, SLOT( on_timeout_timer() ) );
    timer->start( 10 );
}

/******************************************** DESTRUCTOR ***************************************************/
MainWindow::~MainWindow()
{
    delete timer;

    if( timer_label != NULL )
        delete timer_label;

    if( p_puerto != NULL )
    {
        if( p_puerto->isOpen() )
            p_puerto->close();
        delete p_puerto;
    }

    delete ui;
}

/************************************* ACTUALIZAR_LISTA_PUERTOS ********************************************/
void MainWindow::actualizar_lista_puertos()
{
    QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    ui->comboBox_Port->clear();

    for( int i = 0; i < port.size(); i++ )
        ui->comboBox_Port->addItem( port.at(i).portName() );
}

/******************************** ACTUALIZAR_TEXTO_DEZPLAZAMIENTO ******************************************/
void MainWindow::actualizar_texto_desplazamiento( void )
{
    QString text( "Mover " );
    text.append( QString::number( abs( ui->horizontalSlider->value() - VALORCENTRO ) ) );
    text.append( " paso" );

    if( abs( ui->horizontalSlider->value() - VALORCENTRO ) != 1 )
        text.append( "s" );
    if( abs( ui->horizontalSlider->value() - VALORCENTRO ) != 0 )
    {
        if( ui->horizontalSlider->value() < VALORCENTRO )
            text.append( " hacia arriba" );
        else
            text.append( " hacia abajo" );
    }

    ui->pushButton_mover->setText( text );
    ui->actionDesplazar->setText ( text );
}

/********************************************* CONECTAR ****************************************************/
void MainWindow::Conectar()
{
    if( p_puerto == NULL )
        EstadoConectado();
    else
        EstadoDesconectado();
}

/***************************************** ESTADO CONECTADO ************************************************/
void MainWindow::EstadoConectado( void )
{
    p_puerto = new QSerialPort( ui->comboBox_Port->currentText() );

    p_puerto -> setBaudRate   ( QSerialPort::Baud9600 );
    p_puerto -> setDataBits   ( QSerialPort::Data8 );
    p_puerto -> setFlowControl( QSerialPort::NoFlowControl );
    p_puerto -> setStopBits   ( QSerialPort::OneStop );
    p_puerto -> setParity     ( QSerialPort::NoParity );

    if( p_puerto->open( QIODevice::ReadWrite ) == true )
    {
        connect( p_puerto, SIGNAL( readyRead() ), this, SLOT( on_DatosRecibidos() ) );

        ui->pushButton_Conectar->setText( "Desconectar" );
        ui->label_EstadoPort->setStyleSheet( "font-weight: bold; color: red; " );
        ui->label_EstadoPort->setText( "CONECTADO" );
        ui->pushButton_Imprimir->setEnabled( true );
        ui->EditorDeTexto->setFocus();
        ui->groupBox_Papel->setEnabled( true );
    }

    estado_puerto = Conectado;
}

/*************************************** ESTADO DESCONECTADO ***********************************************/
void MainWindow::EstadoDesconectado( void )
{
    p_puerto->close();
    delete p_puerto;
    p_puerto = NULL;

    ui->pushButton_Conectar->setText( "Conectar" );
    ui->label_EstadoPort->setStyleSheet( "font-weight: bold; color: black; " );
    ui->label_EstadoPort->setText( "Desconectado" );
    ui->pushButton_Imprimir->setDisabled( true );
    ui->EditorDeTexto->setFocus();
    ui->groupBox_Papel->setDisabled( true );

    estado_puerto = Desconectado;
}

/****************************************** ESTADOOCUPADO **************************************************/
void MainWindow::EstadoOcupado( void )
{
    ui->groupBox_Puerto    -> setDisabled( true );
    ui->groupBox_Archivo   -> setDisabled( true );
    ui->groupBox_Papel     -> setDisabled( true );
    ui->groupBox_Texto     -> setDisabled( true );

    ui->pushButton_Imprimir-> setDisabled( true );
    ui->pushButton_Salir   -> setDisabled( true );

    estado_puerto = Imprimir;
}

/***************************************** ESTADODESOCUPADO ************************************************/
void MainWindow::EstadoDesocupado( void )
{
    ui->groupBox_Puerto     ->setEnabled( true );
    ui->groupBox_Archivo    ->setEnabled( true );
    ui->groupBox_Papel      ->setEnabled( true );
    ui->groupBox_Texto      ->setEnabled( true );

    ui->pushButton_Imprimir ->setEnabled( true );
    ui->pushButton_Salir    ->setEnabled( true );

    ui->EditorDeTexto       ->setFocus();

    ui->label_EstadoImpresion->setStyleSheet( "font-weight: MS Shell Dlg 2; color: black; " );
    ui->label_EstadoImpresion->setText      ( "Estado: Esperando para imprimir" );

    estado_puerto = Conectado;
}

/*************************************** BUSCARCAMBIODETEXTO ***************************************************/
void MainWindow::BuscarCambioDeTexto( void )    //compara si el texto del editor es igual al texto de un archivo
{
    QFile file( ui->lineEdit_Path->text() );

    if( !file.exists() ) // Si no existe, lo crea
    {
        file.open( QIODevice::WriteOnly  | QIODevice::Text );
        file.close();
    }

    if( !file.open( QIODevice::ReadWrite | QIODevice::Text ) )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Error al abrir el archivo" ) );
        return;
    }

    QTextStream flujoTexto( &file );                 // Lo abro como flujo de texto
    QString textoDelArchivo = flujoTexto.readAll();  // Lo guardo en un stream
    QString textoDelEditor  = ui->EditorDeTexto->toPlainText();

    if( textoDelEditor != textoDelArchivo ) // Compara
    {
        file.resize( textoDelEditor.size() );
        flujoTexto.seek(0);  // Se posiciona en el inicio
        flujoTexto << textoDelEditor;
        file.close();
    }
}

/****************************************** CUADROGUARDAR *****************************************************/
QString MainWindow::CuadroGuardar( void )
{
    QString nombre = QFileDialog::getSaveFileName( this, tr( "Guardar Archivo" ), "\%USERPROFILE\%\\Desktop\\",
                                                   "Texto sin formato (*.txt);;Cualquier Archivo(*.*)" );
    return nombre;
}

/******************************************* CUADROABRIR ******************************************************/
QString MainWindow::CuadroAbrir( void )
{
    QString nombre = QFileDialog::getOpenFileName( this, tr( "Abrir Archivo" ), "\%USERPROFILE\%\\Desktop\\",
                                                   "Texto sin formato (*.txt);;Cualquier Archivo(*.*)" );
    return nombre;
}

/************************************* GUARDARCAMBIOSDELARCHIVO **********************************************/
void MainWindow::GuardarCambiosDelArchivo( void )
{
    QString mensaje;
    QFile   file( ui->lineEdit_Path->text() );

    if( !file.exists() )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "El archivo no existe" ) );
        return;
    }

    if( !file.open( QIODevice::ReadOnly  | QIODevice::Text ) )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "Error al abrir el archivo" ) );
        return;
    }

    QTextStream flujoTexto( &file );
    QString textoDelArchivo = flujoTexto.readAll();
    QString textoDelEditor  = ui->EditorDeTexto->toPlainText();
    file.close();

    // Si hay cambios en el archivo de texto
    if( textoDelEditor != textoDelArchivo )
    {
        if( boton == NUEVO )
            mensaje = "¿Desea guardar los cambios antes de crear un nuevo texto?";
        else
            mensaje = "¿Desea guardar los cambios antes de abrir un nuevo texto?";

        if( QMessageBox::question( this, "Atencion", mensaje, QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
                BuscarCambioDeTexto();  // Guarda los cambios
    }
}

/*************************************** ESTADODELEDITORDETEXTO **********************************************/
void MainWindow::EstadoDelEditorDeTexto( void )
{
    QString mensaje;

    if( boton == NUEVO )
        mensaje = "¿Desea guardar los cambios antes de crear un nuevo texto?";
    else
        mensaje = "¿Desea guardar los cambios antes de abrir un nuevo texto?";

    if( !ui->EditorDeTexto->toPlainText().isEmpty() ) // Si hay texto en el editor de texto
        if( QMessageBox::question( this, "Atencion", mensaje, QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
        {
            QString nombre = CuadroGuardar();
            ui->lineEdit_Path->setText( nombre );
            BuscarCambioDeTexto();
        }
}

/********************************************** AYUDA ********************************************************/
void MainWindow::MostrarAyuda()
{
    p_help = new Ayuda( this );   // Crea memoria dinamica
    p_help->show();               // Abre la ventana de ayuda
}

/******************************************* ARCHIVONUEVO ****************************************************/
void MainWindow::ArchivoNuevo()
{
    boton = NUEVO;

    if( estado == ArchivoAbierto )  // Si hay un archivo abierto
        GuardarCambiosDelArchivo(); // Se modifico?

    if( estado == Editando )        // Si no hay archivo abierto
        EstadoDelEditorDeTexto();   // Hay texto en el editor?

    if( flag_imp == 1 )     // Si previamente se envio a imprimir
    {
        flag_imp = 0;
        ui->EditorDeTexto       -> setEnabled( true );
        ui->pushButton_Imprimir -> setEnabled( true );

        ui->label_Mensaje->setText("");
        ui->label_EstadoImpresion->setStyleSheet( "font-weight: MS Shell Dlg 2; color: black; " );
        ui->label_EstadoImpresion->setText      ( "Estado: Esperando para imprimir" );

        estado_puerto = Conectado;
    }

    setWindowTitle( "Maquina Braille" );
    ui->label_Mensaje->setText("");
    ui->lineEdit_Path->clear();
    ui->EditorDeTexto->clear();
    ui->EditorDeTexto->setFocus();

    estado = Editando;
}

/*********************************************** ABRIR *****************************************************/
void MainWindow::AbrirArchivo()
{
    boton = ABRIR;

    if( estado == ArchivoAbierto )  // Si hay un archivo abierto
        GuardarCambiosDelArchivo(); // Se modifico?

    if( estado == Editando )        // Si no hay archivo abierto
        EstadoDelEditorDeTexto();   // Hay texto en el editor?

    if( flag_imp == 1 )     // Si previamente se envio a imprimir
    {
        flag_imp = 0;
        ui->EditorDeTexto       -> setEnabled( true );
        ui->pushButton_Imprimir -> setEnabled( true );

        ui->label_Mensaje->setText("");
        ui->label_EstadoImpresion->setStyleSheet( "font-weight: MS Shell Dlg 2; color: black; " );
        ui->label_EstadoImpresion->setText      ( "Estado: Esperando para imprimir" );

        estado_puerto = Conectado;
    }

    QString ruta = CuadroAbrir();
    if( !ruta.isEmpty() )
    {
        ui->lineEdit_Path->setText( ruta );

        QFile file( ruta );
        file.open( QIODevice::ReadOnly  | QIODevice::Text );

        QFileInfo path( file.fileName() );
        QString nombreArchivo = path.fileName();
        setWindowTitle( "Maquina Braille - " + nombreArchivo );

        QTextStream flujoTexto( &file );
        QString texto = flujoTexto.readAll();
        counter = texto.size();
        file.close();

        ui->EditorDeTexto->setPlainText( texto );
        ui->EditorDeTexto->moveCursor( QTextCursor::End );
        if( counter > MAX_CARACTERES )
            QMessageBox::critical( this, "ATENCION", "Se supero el limite de caracteres. Por favor edite el archivo" );

        estado = ArchivoAbierto;
    }
    ui->label_Mensaje->setText( "" );
    ui->EditorDeTexto->setFocus();
}

/*********************************************** GUARDAR *****************************************************/
void MainWindow::GuardarArchivo()
{
    char flag = 1;

    if( estado == Editando )
    {
        QString nombre = CuadroGuardar();
        if( nombre.isEmpty() )
            flag = 0;
        else
        {
            ui->lineEdit_Path->setText( nombre );
            QFile file( nombre );
            file.open( QIODevice::ReadWrite | QIODevice::Text );

            QFileInfo path( file.fileName() );
            QString nombreArchivo = path.fileName();
            setWindowTitle( "Maquina Braille - " + nombreArchivo );
            file.close();

            estado = ArchivoAbierto;
        }
    }

    if( flag )
        BuscarCambioDeTexto();

    ui->EditorDeTexto->setFocus();
}

/*********************************************** SALIR ****************************************************/
void MainWindow::Salir()
{
    if( estado == ArchivoAbierto )
    {
        QFile file( ui->lineEdit_Path->text() );
        if( file.open( QIODevice::ReadOnly  | QIODevice::Text ) )
        {
            QTextStream flujoTexto( &file );
            QString textoDelArchivo = flujoTexto.readAll();
            QString textoDelEditor  = ui->EditorDeTexto->toPlainText();
            file.close();

            if( textoDelEditor != textoDelArchivo )
            {
                if( QMessageBox::question( this, "Salir", "¿Desea guardar los cambios antes de salir?",
                                           QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
                {
                    BuscarCambioDeTexto();
                    this->close();
                }
            }
            else if( QMessageBox::question( this, "Salir", "¿Desea salir?",
                                       QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
                this->close();
        }
    }

    else if( estado == Editando )// ui->lineEdit_Path->text().isEmpty() )
    {
        if( QMessageBox::question( this, "Salir", "¿Desea guardar los cambios antes de salir?",
                                   QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
        {
            QString nombre = CuadroGuardar();
            if( nombre.isEmpty() )
                this->close();
            else
            {
                ui->lineEdit_Path->setText( nombre );       // Escribe la ruta del archivo
                QFile file( ui->lineEdit_Path->text() );
                file.open( QIODevice::WriteOnly  | QIODevice::Text ); // Crea el archivo
                file.close();                               // Cierra el archivo
                BuscarCambioDeTexto();
                this->close();
            }
        }
        else
            this->close();
    }
    else if( QMessageBox::question( this, "Salir", "¿Desea salir?",
                                   QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
            this->close();
}

/****************************************** IMPRIMIRTEXTO ****************************************************/
void MainWindow::ImprimirTexto()
{
    QString comando1 = COMANDO_FUNCION;
    QString comando2 = FUNCION_PREGUNTAR_HOJA;

    if( !ui->EditorDeTexto->toPlainText().isEmpty() )
    {
        if( QMessageBox::question( this, "Imprimir", "¿Desea Imprimir el texto actual?",
                                   QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
        {
            p_puerto->write( comando1.toLocal8Bit() );
            p_puerto->write( comando2.toLocal8Bit() );

            ui->label_trama->setText( "Comando Enviado: 0xFF 0xFB" );
            estado_label = Imprimir;
            EstadoOcupado();

            timer_label = new QTimer();
            connect( timer_label, SIGNAL( timeout() ), this, SLOT( on_timeout_label() ) );
            timer_label->start(700);
        }
    }
    else
    {
        ui->label_Mensaje->setText( "ERROR (Archivo vacio)" );
        ui->EditorDeTexto->setFocus();
    }
}

/****************************************** RETIRARPAPEL ***************************************************
 * Si se presiona "Retirar", el programa manda los comandos: "0xFF 0xFD"
 * El LPC debe reconocer el comando y actuar en consecuencia.
 **********************************************************************************************************/
void MainWindow::RetirarPapel( void )
{
    QString comando1 = COMANDO_FUNCION;
    QString comando2 = FUNCION_EXPULSAR;

    p_puerto->write( comando1.toLocal8Bit() );
    p_puerto->write( comando2.toLocal8Bit() );

    ui->label_trama->setText( "Comando Enviado: 0xFF 0xFD" );

    estado_label = Retirando;
    ui->pushButton_Retirar  -> setDisabled( true );
    ui->pushButton_mover    -> setDisabled( true );
    ui->pushButton_Imprimir -> setDisabled( true );

    timer_label = new QTimer();
    connect( timer_label, SIGNAL( timeout() ), this, SLOT( on_timeout_label() ) );
    timer_label->start(700);
}

/*************************************** ON_TIMEOUT_LABEL *************************************************/
void MainWindow::on_timeout_label( void )
{
    static int cont = 0;
    cont++;

    if( ( cont%2 ) == 1 )
    {
        if( estado_label == Retirando )
            ui->label_Mensaje->setText( "EXPULSANDO HOJA" );

        if( estado_label == Moviendo )
            ui->label_Mensaje->setText( "MOVIENDO MOTOR");

        if( estado_label == Imprimir )
            ui->label_Mensaje->setText( "ESPERANDO A LA IMPRESORA");
    }
    else
        ui->label_Mensaje->setText( "" );

    if( cont == 4 )    // 700ms x 4 aprox. 3seg
    {
        cont = 0;
        if( estado_label != Imprimir )
        {
            ui->pushButton_Retirar  -> setEnabled( true );
            ui->pushButton_mover    -> setEnabled( true );
            ui->pushButton_Imprimir -> setEnabled( true );
            ui->label_trama->setText( "Comando Enviado: " );
        }
        else
        {
            ui->label_Mensaje->setText( "No llego confirmacion");
            EstadoDesocupado();
        }

        timer_label->stop();
        delete timer_label;
        timer_label = NULL;
    }
}

/****************************************** AJUSTARMOTOR **************************************************
 * Este boton envia el comando "0xFE" (þ) + el numero de pasos
  *********************************************************************************************************/
void MainWindow::AjustarMotor( void )
{
    QString comando1 = COMANDO_MOVER;
    QString comando2 = QString::number(ui->horizontalSlider->value());
    QByteArray pasos(1,ui->horizontalSlider->value());

    p_puerto->write( comando1.toLocal8Bit() );    
    p_puerto->write( pasos );

    ui->label_trama->setText( "Comando Enviado: 0xFE " + comando2 );
    estado_label = Moviendo;

    ui->pushButton_Retirar  -> setDisabled( true );
    ui->pushButton_mover    -> setDisabled( true );
    ui->pushButton_Imprimir -> setDisabled( true );

    timer_label = new QTimer();
    connect( timer_label, SIGNAL( timeout() ), this, SLOT( on_timeout_label() ) );
    timer_label->start(200);
}

/*************************************** ON_TIMEOUT_TIMER *************************************************/
void MainWindow::on_timeout_timer( void )
{
    QString letras = ui->EditorDeTexto->toPlainText();
    counter = letras.size();

    if( counter > MAX_CARACTERES )
    {
        ui->label_CantidadLetras->setStyleSheet( "font-weight: bold; color: red; " );
        ui->label_CantidadLetras->setText( "SOBREPASO EL LIMITE:   " + QString::number( counter ) + " / " + QString::number( MAX_CARACTERES ) );
        if( estado_puerto == Conectado )
            ui->pushButton_Imprimir->setDisabled( true );
    }

    else if( counter < MAX_CARACTERES )
    {
        ui->label_CantidadLetras->setStyleSheet( "font-weight: bold; color: black; " );
        ui->label_CantidadLetras->setText( QString::number( counter ) + " / " + QString::number( MAX_CARACTERES ) );

    }
    else
    {
        ui->label_CantidadLetras->setStyleSheet( "font-weight: bold; color: red; " );
        ui->label_CantidadLetras->setText( "Llego al límite:   " + QString::number( counter ) + " / " + QString::number( MAX_CARACTERES ) );
        if( estado_puerto == Conectado )
            ui->pushButton_Imprimir->setEnabled( true );
    }
}

/*************************************** ON_DATOSRECIBIDOS *************************************************
 * Debe recibir "Y" para imprimir. Si recibe "N" no imprime
 * "Y" indica que hay papel en la impresora,
 * entre otras cosas para determinar si está lista
  ***********************************************************************************************************/
void MainWindow::on_DatosRecibidos( void )
{
    QByteArray bytes;
    QString enter="\n";

    int cant = p_puerto->bytesAvailable();  // Analizo datos disponibles en el buffer de Rx
    bytes.resize( cant );                   // Redimensiono el QbyteArray considerando '\0'

    p_puerto->read( bytes.data(), bytes.size() );  // Leo y lo guarda en el QbyteArray,
                                                   // data() devuelve un char*
    if( bytes == "Y" )
    {
        p_puerto->write( ui->EditorDeTexto->toPlainText().toLocal8Bit() );  // Envia datos del EditorDeTexto al TX
        p_puerto->write( enter.toLocal8Bit() );         //enviar salto de linea al final

        EstadoDesocupado();
        timer_label->stop();
        delete timer_label;
        timer_label = NULL;
        ui->label_EstadoImpresion->setStyleSheet( "font-weight: bold; color: red; " );
        ui->label_EstadoImpresion->setText( "LA IMPRESORA ESTA LISTA. IMPRIMIENDO..." );

        flag_imp = 1;

    }
    if( bytes == "N" )
    {
        ui->label_Mensaje->setText( "NO HAY HOJAS" );
        EstadoDesocupado();
    }
}












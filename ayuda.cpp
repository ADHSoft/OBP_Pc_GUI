#include "ayuda.h"
#include "ui_ayuda.h"
#include <QString>
#include <iostream>
#include <iomanip>
#include <ostream>

/***************************************************************************************************/
Ayuda::Ayuda(QWidget *parent):QDialog(parent), ui(new Ui::Ayuda)
{
    ui->setupUi(this);

    ui->tabWidget->setTabText( 0, "Nuevo" );
    ui->tabWidget->setTabText( 1, "Abrir" );
    ui->tabWidget->setTabText( 2, "Guardar" );
    ui->tabWidget->setTabText( 3, "Imprimir" );
    ui->tabWidget->setTabText( 4, "Ayuda" );
    ui->tabWidget->setTabText( 5, "Puerto Serie" );
    ui->tabWidget->setTabText( 6, "Ruta" );
    ui->tabWidget->setTabText( 7, "Salir" );

    ui->plainTextEdit_Nuevo->setReadOnly( true );
    ui->plainTextEdit_Abrir->setReadOnly( true );
    ui->plainTextEdit_Guardar->setReadOnly( true );
    ui->plainTextEdit_Imprimir->setReadOnly( true );
    ui->plainTextEdit_Ayuda->setReadOnly( true );
    ui->plainTextEdit_PuertoSerie->setReadOnly( true );
    ui->plainTextEdit_Ruta->setReadOnly( true );
    ui->plainTextEdit_Salir->setReadOnly( true );

    IngresarTextoDeAyuda();
}

/***************************************************************************************************/
Ayuda::~Ayuda()
{
    delete ui;
}

/***************************************************************************************************/
void Ayuda::IngresarTextoDeAyuda( void )
{
    QString texto;

    texto = " -Al hacer clic en <Nuevo>, en caso de que el documento actual no haya tenido cambios desde "
            "su último guardado, abrirá un nuevo documento en blanco.\n\n"
            " -Si el documento actual se modificó y no se guardaron las modificaciones, preguntará si desea guardarlo.";
    ui->plainTextEdit_Nuevo->setPlainText( texto );

    texto = " -Al hacer clic en <Abrir>, se abrirá una ventana donde pedirá que seleccione un archivo de texto.\n\n"
            " -El título de la ventana de windows será \"Maquina Braille + El nombre del archivo\".\n\n"
            " -El contenido del archivo se muestra en el editor de texto, para su edicion y posterior impresión, "
            "si es que se desea.";
    ui->plainTextEdit_Abrir->setPlainText( texto );

    texto = " -Este boton guarda los cambios hechos en el archivo de texto.\n\n"
            " -Si el archivo no fue modificado, ignora el guardado.";
    ui->plainTextEdit_Guardar->setPlainText( texto );

    texto = " -El botón <Imprimir> iniciará automáticamente la impresión de todo el contenido del editor de texto.\n\n"
            " -Este boton inicialmente aparece deshabilitado.\n\n"
            " -Se habilita cuando se conecta el puerto serie.";
    ui->plainTextEdit_Imprimir->setPlainText( texto );

    texto = " -El botón de <Ayuda> proveerá un pequeño manual con instrucciones de uso.";
    ui->plainTextEdit_Ayuda->setPlainText( texto );

    texto = " -Primero se debe selecionar del combo box el puerto que se desea abrir.\n\n"
            " -Luego se debe presionar el boton <Conectar>.\n\n"
            " -El cartel muestra el estado de la coneccion del puerto serie.\n\n"
            " -Si la conexión fue exitosa, muestra la leyenda \"CONECTADO\" en rojo.";
    ui->plainTextEdit_PuertoSerie->setPlainText( texto );

    texto = " -Muestra la ruta de acceso del archivo de texto seleccionado.\n\n"
            " -Esta ruta es de solo lectura.";
    ui->plainTextEdit_Ruta->setPlainText( texto );

    texto = " -Este boton, abre una ventana de dialogo que pregunta si desea cerrar la aplicacion.\n\n"
            " -Al igual que el botón <Nuevo>, si el documento se cierra antes de que se guarden los cambios, "
            "se preguntará si desea guardarlos.";
    ui->plainTextEdit_Salir->setPlainText( texto );
}

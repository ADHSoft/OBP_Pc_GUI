#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ayuda.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

namespace Ui{ class MainWindow; }

typedef enum { Editando, ArchivoAbierto, Conectado, Desconectado, Imprimir, Moviendo, Retirando } estado_t;

class MainWindow : public QMainWindow
{
 Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();
    void BuscarCambioDeTexto( void );
    void EstadoConectado    ( void );
    void EstadoDesconectado ( void );
    void EstadoOcupado   ( void );
    void EstadoDesocupado( void );
    QString CuadroAbrir  ( void );
    QString CuadroGuardar( void );
    void GuardarCambiosDelArchivo( void );
    void EstadoDelEditorDeTexto  ( void );

private slots:
    void Conectar();
    void ArchivoNuevo();
    void AbrirArchivo();
    void GuardarArchivo();
    void MostrarAyuda();
    void Salir();
    void actualizar_texto_desplazamiento( void );
    void actualizar_lista_puertos( void );
    void on_timeout_timer();
    void on_timeout_label( void );
    void AjustarMotor();
    void RetirarPapel();
    void ImprimirTexto();
    void on_DatosRecibidos();

private:
    Ui::MainWindow *ui;
    QSerialPort    *p_puerto;
    Ayuda          *p_help;
    QTimer         *timer;
    QTimer         *timer_label;

    estado_t  estado;
    estado_t  estado_puerto;
    estado_t  estado_label;
    int counter;
    int flag_imp;
    int boton;
};

#endif // MAINWINDOW_H

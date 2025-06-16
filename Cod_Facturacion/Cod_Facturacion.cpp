// --------------------------------------------------------
// Author: Ricardo Márquez
// GitHub: https://github.com/Ricardo21Josee
// LinkedIn: https://www.linkedin.com/in/ric21marquez
// Instagram: @mar_quez_g
// Threads: @mar_quez_g
// Email: josemarquez21garcia@gmail.com
// --------------------------------------------------------

// Definiciones para suprimir warnings comunes
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING

#pragma warning(push)
#pragma warning(disable: 5030)  // Para el warning [[assume]]
#include <pqxx/pqxx>
#pragma warning(pop)

// Resto de includes
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <ctime>
#include <stdexcept>
#include <cstdlib>
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;
using namespace pqxx;

// Estructuras de datos actualizadas segun las tablas
struct Producto {
    int id = 0;
    string nombre = "";
    string descripcion = "";
    double precio = 0.0;
    int existencia = 0;
};

struct Cliente {
    int id = 0;
    string nombre = "";
    string apellido = "";
    string direccion = "";
    string dpi = "";
    string nit = "";
};

struct ClienteCF {
    int id = 0;
    string nombre = "";
    string direccion = "";
};

struct Factura {
    int id = 0;
    string codigo = "";
    string nombreEmpresa = "";
    string direccionEmpresa = "";
    string nitEmpresa = "";
    int cliente_id = 0;
    int cf_cliente_id = 0;
    double subtotal_sin_iva = 0.0;
    double subtotal = 0.0;
    double iva = 0.0;
    double total = 0.0;
    bool requiere_nit = false;
    vector<Producto> productos;
};

// Configuracion de la base de datos
const string DB_NAME = "Tienda_Facturacion_DB";
const string DB_USER = "ricardo";
const string DB_PASSWORD = "ricgio921";
const string DB_HOST = "localhost";
const string DB_PORT = "5432";

// Datos de la empresa
const string NOMBRE_EMPRESA = "TecnoShop GT";
const string DIRECCION_EMPRESA = "12 Calle 1-25 Zona 10, Ciudad de Guatemala";
const string NIT_EMPRESA = "123456-7";

// Configuracion de consola
void configurarConsola() noexcept {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("cls");
#endif
}

// Funcion para limpiar la pantalla
void limpiarPantalla() noexcept {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Conexion a la base de datos
connection conectarDB() {
    try {
        string conn_str =
            "host=" + DB_HOST + " "
            "port=" + DB_PORT + " "
            "dbname=" + DB_NAME + " "
            "user=" + DB_USER + " "
            "password=" + DB_PASSWORD;

        connection C(conn_str);
        if (C.is_open()) {
            cout << "Conexion exitosa a PostgreSQL" << endl;
            return C;
        }
        throw runtime_error("No se pudo conectar a la base de datos");
    }
    catch (const exception& e) {
        cerr << "Error de conexion: " << e.what() << endl;
        cerr << "Verifique:\n"
            << "1. Servicio PostgreSQL activo\n"
            << "2. Credenciales correctas\n"
            << "3. Usuario tenga permisos\n"
            << "4. Firewall permite conexiones al puerto 5432" << endl;
        exit(1);
    }
}

// FunciÃ³n para verificar que las tablas existan
void verificarTablas(connection& C) {
    try {
        work W(C);

        // Verificar tabla productos_inventario
        W.exec("SELECT id FROM productos_inventario LIMIT 1");

        // Verificar tabla clientes
        W.exec("SELECT id FROM clientes LIMIT 1");

        // Verificar tabla clientes_cf
        W.exec("SELECT id FROM clientes_cf LIMIT 1");

        // Verificar tabla facturas
        W.exec("SELECT factura_id FROM facturas LIMIT 1");

        // Verificar tabla pre_detalle_factura
        W.exec("SELECT id_pre_factura FROM pre_detalle_factura LIMIT 1");

        W.commit();
    }
    catch (const exception& e) {
        cerr << "Error: Una o mas tablas no existen en la base de datos. " << e.what() << endl;
        exit(1);
    }
}

// Obtener inventario completo desde la base de datos ordenado por ID
vector<Producto> obtenerInventario(connection& C) {
    vector<Producto> inventario;
    try {
        work W(C);
        result R = W.exec("SELECT id, nombre, descripcion, precio, existencia FROM productos_inventario ORDER BY id");

        for (auto row : R) {
            Producto p;
            p.id = row[0].as<int>();
            p.nombre = row[1].as<string>();
            p.descripcion = row[2].as<string>();
            p.precio = row[3].as<double>();
            p.existencia = row[4].as<int>();
            inventario.push_back(p);
        }
        W.commit();
    }
    catch (const exception& e) {
        cerr << "Error al obtener inventario: " << e.what() << endl;
    }
    return inventario;
}

// Obtener un producto especÃ­fico por ID
Producto obtenerProductoPorId(connection& C, int id) {
    try {
        work W(C);
        result R = W.exec("SELECT id, nombre, descripcion, precio, existencia FROM productos_inventario WHERE id = " + to_string(id));

        if (R.empty()) {
            throw runtime_error("Producto no encontrado");
        }

        Producto p;
        auto row = R[0];
        p.id = row[0].as<int>();
        p.nombre = row[1].as<string>();
        p.descripcion = row[2].as<string>();
        p.precio = row[3].as<double>();
        p.existencia = row[4].as<int>();

        W.commit();
        return p;
    }
    catch (const exception& e) {
        cerr << "Error al obtener producto: " << e.what() << endl;
        throw;
    }
}

// Actualizar existencias en la base de datos
bool actualizarExistencias(connection& C, int producto_id, int nueva_existencia) {
    try {
        work W(C);
        W.exec("UPDATE productos_inventario SET existencia = " + to_string(nueva_existencia) +
            " WHERE id = " + to_string(producto_id));
        W.commit();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error al actualizar existencias: " << e.what() << endl;
        return false;
    }
}

// Reducir existencias cuando se vende un producto
bool reducirExistencias(connection& C, int producto_id, int cantidad) {
    try {
        work W(C);

        // Verificar que haya suficiente existencia primero
        result R = W.exec("SELECT existencia FROM productos_inventario WHERE id = " + to_string(producto_id));
        if (R.empty()) {
            throw runtime_error("Producto no encontrado");
        }

        int existencia_actual = R[0][0].as<int>();
        if (existencia_actual < cantidad) {
            throw runtime_error("No hay suficiente existencia");
        }

        W.exec("UPDATE productos_inventario SET existencia = existencia - " + to_string(cantidad) +
            " WHERE id = " + to_string(producto_id));
        W.commit();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error al reducir existencias: " << e.what() << endl;
        return false;
    }
}

// Agregar nuevo producto al inventario
bool agregarProducto(connection& C, const Producto& p) {
    try {
        work W(C);
        string query = "INSERT INTO productos_inventario (nombre, descripcion, precio, existencia) VALUES (" +
            W.quote(p.nombre) + ", " + W.quote(p.descripcion) + ", " +
            to_string(p.precio) + ", " + to_string(p.existencia) + ")";
        W.exec(query);
        W.commit();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error al agregar producto: " << e.what() << endl;
        return false;
    }
}

// Mostrar el inventario actual ordenado por ID
void mostrarInventario(const vector<Producto>& inventario) {
    cout << "\n=== INVENTARIO ACTUAL ===" << endl;
    cout << setw(5) << left << "ID"
        << setw(25) << left << "Nombre"
        << setw(40) << left << "Descripcion"
        << setw(10) << right << "Precio"
        << setw(10) << right << "Existencia" << endl;
    cout << string(90, '-') << endl;

    for (const auto& p : inventario) {
        cout << setw(5) << left << p.id
            << setw(25) << left << p.nombre
            << setw(40) << left << (p.descripcion.length() > 35 ? p.descripcion.substr(0, 35) + "..." : p.descripcion)
            << setw(10) << right << fixed << setprecision(2) << p.precio
            << setw(10) << right << p.existencia << endl;
    }
}

// Funcion para guardar una factura en la base de datos
bool guardarFactura(connection& C, Factura& factura, const string& nombreCliente, const string& nit = "", const string& direccion = "", const string& dpi = "") {
    try {
        work W(C);

        // Insertar cliente (segun si tiene NIT o no)
        int cliente_id = 0;
        int cf_cliente_id = 0;

        if (factura.requiere_nit) {
            // Insertar cliente con NIT
            string query = "INSERT INTO clientes (nombre, apellido, direccion, dpi, nit) VALUES (" +
                W.quote(nombreCliente) + ", '', " +
                W.quote(direccion) + ", " +
                W.quote(dpi) + ", " +
                W.quote(nit) + ") RETURNING id";
            result R = W.exec(query);
            cliente_id = R[0][0].as<int>();
        }
        else {
            // Insertar consumidor final
            string query = "INSERT INTO clientes_cf (nombre, direccion) VALUES (" +
                W.quote(nombreCliente) + ", " +
                W.quote(direccion) + ") RETURNING id";
            result R = W.exec(query);
            cf_cliente_id = R[0][0].as<int>();
        }

        // Insertar factura
        string factura_query = "INSERT INTO facturas (nombre_empresa, direccion_empresa, nit_empresa, codigo_factura, "
            "cliente_id, cf_cliente_id, subtotal_sin_iva, subtotal, iva, total, requiere_nit) "
            "VALUES (" +
            W.quote(factura.nombreEmpresa) + ", " +
            W.quote(factura.direccionEmpresa) + ", " +
            W.quote(factura.nitEmpresa) + ", " +
            W.quote(factura.codigo) + ", " +
            (cliente_id > 0 ? to_string(cliente_id) : "NULL") + ", " +
            (cf_cliente_id > 0 ? to_string(cf_cliente_id) : "NULL") + ", " +
            to_string(factura.subtotal_sin_iva) + ", " +
            to_string(factura.subtotal) + ", " +
            to_string(factura.iva) + ", " +
            to_string(factura.total) + ", " +
            (factura.requiere_nit ? "TRUE" : "FALSE") + ") RETURNING factura_id";

        result factura_res = W.exec(factura_query);
        int factura_id = factura_res[0][0].as<int>();

        // Insertar detalles de factura
        for (const auto& producto : factura.productos) {
            string detalle_query = "INSERT INTO pre_detalle_factura (factura_id, producto_id, cantidad, precio_unitario, subtotal) "
                "VALUES (" +
                to_string(factura_id) + ", " +
                to_string(producto.id) + ", " +
                to_string(producto.existencia) + ", " +
                to_string(producto.precio) + ", " +
                to_string(producto.precio * producto.existencia) + ")";
            W.exec(detalle_query);
        }

        W.commit();
        return true;
    }
    catch (const exception& e) {
        cerr << "Error al guardar factura: " << e.what() << endl;
        return false;
    }
}

// MenÃº para gestionar el inventario
void menuInventario(connection& C) {
    int opcion;
    do {
        limpiarPantalla();
        cout << "=== GESTIoN DE INVENTARIO ===" << endl;
        cout << "1. Ver inventario completo" << endl;
        cout << "2. Buscar producto por ID" << endl;
        cout << "3. Agregar nuevo producto" << endl;
        cout << "4. Actualizar existencias" << endl;
        cout << "5. Volver al menu principal" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            auto inventario = obtenerInventario(C);
            mostrarInventario(inventario);
            break;
        }
        case 2: {
            int id;
            cout << "Ingrese el ID del producto: ";
            cin >> id;
            cin.ignore();

            try {
                Producto p = obtenerProductoPorId(C, id);
                cout << "\nDetalles del producto:" << endl;
                cout << "ID: " << p.id << endl;
                cout << "Nombre: " << p.nombre << endl;
                cout << "Descripcion: " << p.descripcion << endl;
                cout << "Precio: " << fixed << setprecision(2) << p.precio << endl;
                cout << "Existencia: " << p.existencia << endl;
            }
            catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
            break;
        }
        case 3: {
            Producto nuevo;
            cout << "Nombre del producto: ";
            getline(cin, nuevo.nombre);
            cout << "Descripcion: ";
            getline(cin, nuevo.descripcion);
            cout << "Precio: ";
            cin >> nuevo.precio;
            cout << "Existencia inicial: ";
            cin >> nuevo.existencia;
            cin.ignore();

            if (agregarProducto(C, nuevo)) {
                cout << "Producto agregado exitosamente!" << endl;
            }
            else {
                cout << "Error al agregar el producto" << endl;
            }
            break;
        }
        case 4: {
            int id, nueva_existencia;
            cout << "ID del producto a actualizar: ";
            cin >> id;
            cout << "Nueva cantidad en existencia: ";
            cin >> nueva_existencia;
            cin.ignore();

            if (actualizarExistencias(C, id, nueva_existencia)) {
                cout << "Existencias actualizadas correctamente!" << endl;
            }
            else {
                cout << "Error al actualizar existencias" << endl;
            }
            break;
        }
        case 5:
            return;
        default:
            cout << "Opcion no valida!" << endl;
        }

        cout << "\nPresione Enter para continuar...";
        cin.ignore();
    } while (true);
}

// Funcion para emitir una nueva factura
void emitirFactura(connection& C) {
    limpiarPantalla();
    auto inventario = obtenerInventario(C);

    Factura factura;
    factura.nombreEmpresa = NOMBRE_EMPRESA;
    factura.direccionEmpresa = DIRECCION_EMPRESA;
    factura.nitEmpresa = NIT_EMPRESA;
    factura.cliente_id = 0;
    factura.cf_cliente_id = 0;

    // Seleccion de cliente
    cout << "=== DATOS DEL CLIENTE ===" << endl;
    cout << "Nombre del cliente: ";
    string nombreCliente;
    getline(cin, nombreCliente);

    cout << "Direccion: ";
    string direccion;
    getline(cin, direccion);

    cout << "NIT (deje en blanco para consumidor final): ";
    string nit;
    getline(cin, nit);

    string dpi;
    factura.requiere_nit = !nit.empty();

    if (factura.requiere_nit) {
        cout << "DPI (13 di­gitos): ";
        getline(cin, dpi);

        // Validar que el DPI tenga 13 dÃ­gitos
        if (dpi.length() != 13 || !all_of(dpi.begin(), dpi.end(), ::isdigit)) {
            cout << "Error: El DPI debe tener exactamente 13 di­gitos numericos." << endl;
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            return;
        }
    }

    // Seleccion de productos
    char continuar;
    do {
        limpiarPantalla();
        mostrarInventario(inventario);

        int producto_id, cantidad;
        cout << "\nID del producto a facturar (0 para terminar): ";
        cin >> producto_id;

        if (producto_id == 0) break;

        cout << "Cantidad: ";
        cin >> cantidad;
        cin.ignore();

        try {
            Producto p = obtenerProductoPorId(C, producto_id);

            if (p.existencia < cantidad) {
                cout << "No hay suficiente existencia. Disponible: " << p.existencia << endl;
                continue;
            }

            p.existencia = cantidad;
            factura.productos.push_back(p);

            // Reducir existencias en la base de datos
            if (reducirExistencias(C, producto_id, cantidad)) {
                cout << "Producto agregado a la factura" << endl;
            }

        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }

        cout << "\nDesea agregar otro producto? (s/n): ";
        cin >> continuar;
        cin.ignore();
    } while (continuar == 's' || continuar == 'S');

    if (factura.productos.empty()) {
        cout << "Factura vaci­a, no se puede emitir" << endl;
        return;
    }

    // Calcular totales
    factura.subtotal = 0;
    for (const auto& p : factura.productos) {
        factura.subtotal += p.precio * p.existencia;
    }
    factura.subtotal_sin_iva = factura.subtotal / 1.12;
    factura.iva = factura.subtotal_sin_iva * 0.12;
    factura.total = factura.subtotal;

    // Generar codigo de factura mas Unico
    srand(time(nullptr));
    factura.codigo = "FAC-" + to_string(time(nullptr)) + "-" + to_string(rand() % 1000);

    // Mostrar resumen
    limpiarPantalla();
    cout << "\n=== RESUMEN DE FACTURA ===" << endl;
    cout << "Empresa: " << factura.nombreEmpresa << endl;
    cout << "Direccion: " << factura.direccionEmpresa << endl;
    cout << "NIT: " << factura.nitEmpresa << endl;
    cout << "Codigo de factura: " << factura.codigo << endl;
    cout << "\nCliente: " << nombreCliente << endl;
    cout << "Direccion: " << direccion << endl;

    if (factura.requiere_nit) {
        cout << "NIT: " << nit << endl;
        cout << "DPI: " << dpi << endl;
    }
    else {
        cout << "Consumidor Final" << endl;
    }

    cout << "\nProductos:" << endl;
    cout << setw(5) << left << "ID"
        << setw(25) << left << "Nombre"
        << setw(10) << right << "Cantidad"
        << setw(15) << right << "Precio"
        << setw(15) << right << "Subtotal" << endl;
    cout << string(70, '-') << endl;

    for (const auto& p : factura.productos) {
        cout << setw(5) << left << p.id
            << setw(25) << left << p.nombre
            << setw(10) << right << p.existencia
            << setw(15) << right << fixed << setprecision(2) << p.precio
            << setw(15) << right << (p.precio * p.existencia) << endl;
    }

    cout << string(70, '-') << endl;
    cout << setw(55) << right << "Subtotal sin IVA: " << fixed << setprecision(2) << factura.subtotal_sin_iva << endl;
    cout << setw(55) << right << "IVA (12%): " << fixed << setprecision(2) << factura.iva << endl;
    cout << setw(55) << right << "TOTAL: " << fixed << setprecision(2) << factura.total << endl;

    // Guardar la factura en la base de datos
    if (guardarFactura(C, factura, nombreCliente, nit, direccion, dpi)) {
        cout << "\nFactura guardada en la base de datos con codigo: " << factura.codigo << endl;
    }
    else {
        cout << "\nError al guardar la factura en la base de datos" << endl;
    }
}

// MenÃº principal
void menuPrincipal(connection& C) {
    int opcion;
    do {
        limpiarPantalla();
        cout << "=== SISTEMA DE FACTURACION ===" << endl;
        cout << "1. Gestion de Inventario" << endl;
        cout << "2. Emitir Factura" << endl;
        cout << "3. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1:
            menuInventario(C);
            break;
        case 2:
            emitirFactura(C);
            break;
        case 3:
            cout << "Saliendo del sistema..." << endl;
            return;
        default:
            cout << "Opcion no valida!" << endl;
        }

        cout << "\nPresione Enter para continuar...";
        cin.ignore();
    } while (true);
}

int main() {
    configurarConsola();

    try {
        connection C = conectarDB();
        verificarTablas(C);  // Verificar que todas las tablas existan
        menuPrincipal(C);
    }
    catch (const exception& e) {
        cerr << "Error fatal: " << e.what() << endl;
    }

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}

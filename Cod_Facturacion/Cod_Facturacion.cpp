#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib> // Para system("cls") o system("clear")
#include <fstream> // Para manejo de archivos
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Estructuras de datos
struct Producto {
    string nombre;
    string descripcion;
    double precio = 0;
    int cantidad = 0;
    int existencia = 0;
};

struct Cliente {
    string nombre;
    string nit;
};

struct Factura {
    string codigo;
    string nombreEmpresa;
    string direccionEmpresa;
    string nitEmpresa;
    Cliente cliente;
    vector<Producto> productos;
};

// Datos de la empresa
const string NOMBRE_EMPRESA = "TecnoShop GT";
const string DIRECCION_EMPRESA = "12 Calle 1-25 Zona 10, Ciudad de Guatemala";
const string NIT_EMPRESA = "123456-7";

// Vectores globales para almacenar datos
vector<Producto> productosDisponibles;
vector<Factura> facturas;

// Funciones de comparaci髇 para ordenar facturas
bool compararPorCodigo(const Factura& a, const Factura& b) {
    return a.codigo < b.codigo;
}

bool compararPorNombreCliente(const Factura& a, const Factura& b) {
    return a.cliente.nombre < b.cliente.nombre;
}

bool compararPorTotal(const Factura& a, const Factura& b) {
    return a.total < b.total;
}

// Funciones
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Conexi贸n a la base de datos
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
            cout << "Conexi贸n exitosa a PostgreSQL" << endl;
            return C;
        }
        throw runtime_error("No se pudo conectar a la base de datos");
    }
    catch (const exception& e) {
        cerr << "Error de conexi贸n: " << e.what() << endl;
        cerr << "Verifique:\n"
            << "1. Servicio PostgreSQL activo\n"
            << "2. Credenciales correctas\n"
            << "3. Usuario tenga permisos\n"
            << "4. Firewall permite conexiones al puerto 5432" << endl;
        exit(1);
}

void cargarDatos() {
    ifstream file("datos.json");
    if (!file.is_open()) {
        // Si el archivo no existe, creamos uno vac韔
        productosDisponibles = {};
        facturas = {};
        guardarDatos();
        return;
    }

// Obtener inventario completo desde la base de datos
vector<Producto> obtenerInventario(connection& C) {
    vector<Producto> inventario;
    try {
        work W(C);
        result R = W.exec("SELECT id, nombre, descripcion, precio, existencia FROM productos_inventario ORDER BY nombre");

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

// Obtener un producto espec铆fico por ID
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
    cout << "----------------------------------------" << endl;
    cout << "Productos:" << endl;
    cout << setw(20) << left << "Nombre" << setw(10) << "Precio" << setw(10) << "Cantidad" << setw(10) << "Total" << endl;
    for (const auto& producto : factura.productos) {
        cout << setw(20) << left << producto.nombre
            << setw(10) << producto.precio
            << setw(10) << producto.cantidad
            << setw(10) << producto.precio * producto.cantidad << endl;
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

// Mostrar el inventario actual
void mostrarInventario(const vector<Producto>& inventario) {
    cout << "\n=== INVENTARIO ACTUAL ===" << endl;
    cout << setw(5) << left << "ID"
        << setw(25) << left << "Nombre"
        << setw(10) << right << "Precio"
        << setw(10) << right << "Existencia" << endl;
    cout << string(50, '-') << endl;

    for (const auto& p : inventario) {
        cout << setw(5) << left << p.id
            << setw(25) << left << p.nombre
            << setw(10) << right << fixed << setprecision(2) << p.precio
            << setw(10) << right << p.existencia << endl;
    }
}

// Men煤 para gestionar el inventario
void menuInventario(connection& C) {
    int opcion;
    do {
        limpiarPantalla();
        cout << "=== GESTI脫N DE INVENTARIO ===" << endl;
        cout << "1. Ver inventario completo" << endl;
        cout << "2. Buscar producto por ID" << endl;
        cout << "3. Agregar nuevo producto" << endl;
        cout << "4. Actualizar existencias" << endl;
        cout << "5. Volver al men煤 principal" << endl;
        cout << "Seleccione una opci贸n: ";
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
                cout << "Descripci贸n: " << p.descripcion << endl;
                cout << "Precio: " << fixed << setprecision(2) << p.precio << endl;
                cout << "Existencia: " << p.existencia << endl;
            }
            catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
        }
        case 2: {
            if (factura.productos.empty()) {
                cout << "------------------------------------------------" << endl;
                cout << "No hay productos para eliminar." << endl;
                break;
            }
        case 3: {
            Producto nuevo;
            cout << "Nombre del producto: ";
            getline(cin, nuevo.nombre);
            cout << "Descripci贸n: ";
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

            // Eliminar el producto de la factura
            factura.productos.erase(factura.productos.begin() + idEliminar - 1);
            calcularTotales(factura);
            break;
        }
        case 3: {
            factura.codigo = "FAC-" + to_string(facturas.size() + 1);
            facturas.push_back(factura);
            guardarDatos(); // Guardar datos despu閟 de emitir la factura
            cout << "--------------------------------------" << endl;
            cout << "Factura emitida con codigo: " << factura.codigo << endl;
            return;
        }
        case 4: {
            devolverProductosAlInventario(factura);
            guardarDatos(); // Guardar datos despu閟 de cancelar la factura
            cout << "------------------------------------------------------------------" << endl;
            cout << "Factura cancelada. Los productos han sido devueltos al inventario." << endl;
            return;
        }
        default:
            cout << "Opci贸n no v谩lida!" << endl;
        }

        cout << "\nPresione Enter para continuar...";
        cin.ignore();
    } while (true);
}

// Funci贸n para emitir una nueva factura
void emitirFactura(connection& C) {
    limpiarPantalla();
    auto inventario = obtenerInventario(C);

    Factura factura;
    factura.nombreEmpresa = NOMBRE_EMPRESA;
    factura.direccionEmpresa = DIRECCION_EMPRESA;
    factura.nitEmpresa = NIT_EMPRESA;
    factura.cliente_id = 0;
    factura.cf_cliente_id = 0;

    // Selecci贸n de cliente (simplificado para el ejemplo)
    cout << "=== DATOS DEL CLIENTE ===" << endl;
    cout << "Nombre del cliente: ";
    string nombreCliente;
    getline(cin, nombreCliente);

    cout << "NIT (deje en blanco para consumidor final): ";
    string nit;
    getline(cin, nit);

    factura.requiere_nit = !nit.empty();

    // Selecci贸n de productos
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
    else {
        // Preguntar si desea agregar NIT para facturas menores a 2499
        char opcionNIT;
        cout << "---------------------------------------------------------" << endl;
        cout << "El monto total es menor a Q2499. Desea:" << endl;
        cout << "1. Agregar NIT (ingrese 'n')" << endl;
        cout << "2. Usar CF - Consumidor Final (ingrese 'c')" << endl;
        cout << "Seleccione una opcion (n/c): ";
        cin >> opcionNIT;
        cin.ignore();

        if (opcionNIT == 'n' || opcionNIT == 'N') {
            cout << "Ingrese el NIT del cliente: ";
            getline(cin, factura.cliente.nit);
            factura.requiereNIT = true;
        }
        else {
            factura.cliente.nit = "CF";
            factura.requiereNIT = false;
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
    }

        cout << "\nDesea agregar otro producto? (s/n): ";
        cin >> continuar;
        cin.ignore();
    } while (continuar == 's' || continuar == 'S');

    if (factura.productos.empty()) {
        cout << "Factura vac铆a, no se puede emitir" << endl;
            return;
        }
    }

    cout << "Factura no encontrada." << endl;
}

    // Calcular totales
    factura.subtotal = 0;
    for (const auto& p : factura.productos) {
        factura.subtotal += p.precio * p.existencia;
    }
    factura.subtotal_sin_iva = factura.subtotal / 1.12;
    factura.iva = factura.subtotal_sin_iva * 0.12;
    factura.total = factura.subtotal;

    // Generar c贸digo de factura
    factura.codigo = "FAC-" + to_string(time(nullptr));

    // Mostrar resumen
    limpiarPantalla();
    cout << "\n=== RESUMEN DE FACTURA ===" << endl;
    cout << "Cliente: " << nombreCliente << endl;
    if (factura.requiere_nit) {
        cout << "NIT: " << nit << endl;
    }
    else {
        cout << "Consumidor Final" << endl;
}

    cout << "\nProductos:" << endl;
    cout << setw(25) << left << "Nombre"
        << setw(10) << right << "Cantidad"
        << setw(15) << right << "Precio"
        << setw(15) << right << "Subtotal" << endl;
    cout << string(65, '-') << endl;

    for (const auto& p : factura.productos) {
        cout << setw(25) << left << p.nombre
            << setw(10) << right << p.existencia
            << setw(15) << right << fixed << setprecision(2) << p.precio
            << setw(15) << right << (p.precio * p.existencia) << endl;
    }

    cout << string(65, '-') << endl;
    cout << setw(50) << right << "Subtotal sin IVA: " << fixed << setprecision(2) << factura.subtotal_sin_iva << endl;
    cout << setw(50) << right << "IVA (12%): " << fixed << setprecision(2) << factura.iva << endl;
    cout << setw(50) << right << "TOTAL: " << fixed << setprecision(2) << factura.total << endl;

    // Aqu铆 deber铆as implementar la inserci贸n en las tablas facturas y pre_detalle_factura
    cout << "\nFactura generada con c贸digo: " << factura.codigo << endl;
}

// Men煤 principal
void menuPrincipal(connection& C) {
    int opcion;
    do {
        limpiarPantalla();
        cout << "=== SISTEMA DE FACTURACI脫N ===" << endl;
        cout << "1. Gesti贸n de Inventario" << endl;
        cout << "2. Emitir Factura" << endl;
        cout << "3. Salir" << endl;
        cout << "Seleccione una opci贸n: ";
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
            cout << "Opcion no valida. Intente de nuevo." << endl;
        }

        cout << "\nPresione Enter para continuar...";
        cin.ignore();
    } while (true);
}

int main() {
    configurarConsola();

    try {
        connection C = conectarDB();
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
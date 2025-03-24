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
    double siva = 0;
    double subtotal = 0;
    double iva = 0;
    double total = 0;
    bool requiereNIT = false;
};

// Datos de la empresa
const string NOMBRE_EMPRESA = "TecnoShop GT";
const string DIRECCION_EMPRESA = "12 Calle 1-25 Zona 10, Ciudad de Guatemala";
const string NIT_EMPRESA = "123456-7";

// Vectores globales para almacenar datos
vector<Producto> productosDisponibles;
vector<Factura> facturas;

// Funciones de comparación para ordenar facturas
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

void guardarDatos() {
    json j;
    j["productos"] = json::array();
    for (const auto& producto : productosDisponibles) {
        j["productos"].push_back({
            {"nombre", producto.nombre},
            {"descripcion", producto.descripcion},
            {"precio", producto.precio},
            {"cantidad", producto.cantidad},
            {"existencia", producto.existencia}
            });
    }

    j["facturas"] = json::array();
    for (const auto& factura : facturas) {
        json jFactura;
        jFactura["codigo"] = factura.codigo;
        jFactura["nombreEmpresa"] = factura.nombreEmpresa;
        jFactura["direccionEmpresa"] = factura.direccionEmpresa;
        jFactura["nitEmpresa"] = factura.nitEmpresa;
        jFactura["cliente"] = {
            {"nombre", factura.cliente.nombre},
            {"nit", factura.cliente.nit}
        };
        jFactura["productos"] = json::array();
        for (const auto& producto : factura.productos) {
            jFactura["productos"].push_back({
                {"nombre", producto.nombre},
                {"descripcion", producto.descripcion},
                {"precio", producto.precio},
                {"cantidad", producto.cantidad},
                {"existencia", producto.existencia}
                });
        }
        jFactura["siva"] = factura.siva;
        jFactura["subtotal"] = factura.subtotal;
        jFactura["iva"] = factura.iva;
        jFactura["total"] = factura.total;
        jFactura["requiereNIT"] = factura.requiereNIT;
        j["facturas"].push_back(jFactura);
    }

    ofstream file("datos.json");
    file << j.dump(4);
    file.close();
}

void cargarDatos() {
    ifstream file("datos.json");
    if (!file.is_open()) {
        // Si el archivo no existe, creamos uno vacío
        productosDisponibles = {};
        facturas = {};
        guardarDatos();
        return;
    }

    json j;
    file >> j;
    file.close();

    productosDisponibles.clear();
    for (const auto& item : j["productos"]) {
        Producto producto;
        producto.nombre = item["nombre"];
        producto.descripcion = item["descripcion"];
        producto.precio = item["precio"];
        producto.cantidad = item["cantidad"];
        producto.existencia = item["existencia"];
        productosDisponibles.push_back(producto);
    }

    facturas.clear();
    for (const auto& item : j["facturas"]) {
        Factura factura;
        factura.codigo = item["codigo"];
        factura.nombreEmpresa = item["nombreEmpresa"];
        factura.direccionEmpresa = item["direccionEmpresa"];
        factura.nitEmpresa = item["nitEmpresa"];
        factura.cliente.nombre = item["cliente"]["nombre"];
        factura.cliente.nit = item["cliente"]["nit"];
        for (const auto& prod : item["productos"]) {
            Producto producto;
            producto.nombre = prod["nombre"];
            producto.descripcion = prod["descripcion"];
            producto.precio = prod["precio"];
            producto.cantidad = prod["cantidad"];
            producto.existencia = prod["existencia"];
            factura.productos.push_back(producto);
        }
        factura.siva = item["siva"];
        factura.subtotal = item["subtotal"];
        factura.iva = item["iva"];
        factura.total = item["total"];
        factura.requiereNIT = item["requiereNIT"];
        facturas.push_back(factura);
    }
}

void mostrarProductosDisponibles() {
    cout << "\n=== PRODUCTOS DISPONIBLES ===" << endl;
    cout << setw(5) << left << "ID"
        << setw(20) << left << "Nombre"
        << setw(50) << left << "Descripcion"
        << setw(10) << left << "Precio"
        << setw(10) << left << "Existencia" << endl;

    for (size_t i = 0; i < productosDisponibles.size(); i++) {
        cout << setw(5) << left << i + 1
            << setw(20) << left << productosDisponibles[i].nombre
            << setw(50) << left << productosDisponibles[i].descripcion
            << setw(10) << left << productosDisponibles[i].precio
            << setw(10) << left << productosDisponibles[i].existencia << endl;
    }
}

void mostrarFactura(const Factura& factura) {
    cout << "\n\n=== " << factura.nombreEmpresa << " ===" << endl;
    cout << "Direccion: " << factura.direccionEmpresa << endl;
    cout << "NIT: " << factura.nitEmpresa << endl;
    cout << "Codigo de Factura: " << factura.codigo << endl;
    cout << "----------------------------------------" << endl;
    cout << "Datos del cliente:" << endl;
    cout << "Nombre: " << factura.cliente.nombre << endl;
    if (factura.requiereNIT) {
        cout << "NIT: " << factura.cliente.nit << endl;
    }
    else {
        cout << "Factura CF (Consumidor Final)" << endl;
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
    cout << "----------------------------------------" << endl;
    cout << setw(40) << left << "Subtotal:" << setw(10) << factura.subtotal << endl;
    cout << "----------------------------------------" << endl;
    cout << setw(40) << left << "Subtotal sin IVA:" << setw(10) << factura.siva << endl;
    cout << setw(40) << left << "IVA (12%):" << setw(10) << factura.iva << endl;
    cout << setw(40) << left << "Total a pagar:" << setw(10) << factura.total << endl;
    cout << "========================================" << endl;
}

void calcularTotales(Factura& factura) {
    factura.subtotal = 0;
    for (const auto& producto : factura.productos) {
        factura.subtotal += producto.precio * producto.cantidad;
    }
    factura.siva = factura.subtotal / 1.12;
    factura.iva = factura.siva * 0.12;
    factura.total = factura.siva + factura.iva;
    factura.requiereNIT = (factura.total > 2499);
}

void devolverProductosAlInventario(Factura& factura) {
    for (auto& productoFactura : factura.productos) {
        for (auto& productoInventario : productosDisponibles) {
            if (productoFactura.nombre == productoInventario.nombre) {
                productoInventario.existencia += productoFactura.cantidad;
                break;
            }
        }
    }
}

void menuFacturacion(Factura& factura) {
    int opcion;
    do {
        limpiarPantalla();
        mostrarFactura(factura);
        cout << "\n=== OPCIONES DE FACTURACION ===" << endl;
        cout << "1. Agregar otro producto" << endl;
        cout << "2. Eliminar un producto" << endl;
        cout << "3. Emitir factura" << endl;
        cout << "4. Cancelar factura" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            limpiarPantalla();
            mostrarProductosDisponibles();
            int idProducto, cantidad;
            cout << " " << endl;
            cout << "------------------------------------------------" << endl;
            cout << "Ingrese el ID del producto que desea agregar: ";
            cin >> idProducto;
            cout << "------------------------------------------------" << endl;
            cout << "Ingrese la cantidad: ";
            cin >> cantidad;

            if (idProducto < 1 || idProducto > productosDisponibles.size()) {
                cout << "------------------------------------------------" << endl;
                cout << "ID de producto no valido. Intente de nuevo." << endl;
                break;
            }

            if (cantidad > productosDisponibles[idProducto - 1].existencia) {
                cout << "------------------------------------------------" << endl;
                cout << "No hay suficiente existencia. Intente de nuevo." << endl;
                break;
            }

            Producto productoSeleccionado = productosDisponibles[idProducto - 1];
            productoSeleccionado.cantidad = cantidad;
            factura.productos.push_back(productoSeleccionado);

            productosDisponibles[idProducto - 1].existencia -= cantidad;
            calcularTotales(factura);
            break;
        }
        case 2: {
            if (factura.productos.empty()) {
                cout << "------------------------------------------------" << endl;
                cout << "No hay productos para eliminar." << endl;
                break;
            }
            cout << "------------------------------------------------" << endl;
            cout << "Ingrese el numero del producto que desea eliminar: ";
            int idEliminar;
            cin >> idEliminar;

            if (idEliminar < 1 || idEliminar > factura.productos.size()) {
                cout << "------------------------------------------------" << endl;
                cout << "Numero de producto no valido." << endl;
                break;
            }

            // Devolver el producto al inventario
            for (auto& productoInventario : productosDisponibles) {
                if (productoInventario.nombre == factura.productos[idEliminar - 1].nombre) {
                    productoInventario.existencia += factura.productos[idEliminar - 1].cantidad;
                    break;
                }
            }

            // Eliminar el producto de la factura
            factura.productos.erase(factura.productos.begin() + idEliminar - 1);
            calcularTotales(factura);
            break;
        }
        case 3: {
            factura.codigo = "FAC-" + to_string(facturas.size() + 1);
            facturas.push_back(factura);
            guardarDatos(); // Guardar datos después de emitir la factura
            cout << "--------------------------------------" << endl;
            cout << "Factura emitida con codigo: " << factura.codigo << endl;
            return;
        }
        case 4: {
            devolverProductosAlInventario(factura);
            guardarDatos(); // Guardar datos después de cancelar la factura
            cout << "------------------------------------------------------------------" << endl;
            cout << "Factura cancelada. Los productos han sido devueltos al inventario." << endl;
            return;
        }
        default:
            cout << "-----------------------------------" << endl;
            cout << "Opcion no valida. Intente de nuevo." << endl;
            cout << " " << endl;
        }
        cout << "-------------------------------" << endl;
        cout << "Presione Enter para continuar...";
        cin.get();
    } while (true);
}

void emitirFactura() {
    Factura factura;
    factura.nombreEmpresa = NOMBRE_EMPRESA;
    factura.direccionEmpresa = DIRECCION_EMPRESA;
    factura.nitEmpresa = NIT_EMPRESA;

    cout << " " << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Ingrese el nombre del cliente: ";
    getline(cin, factura.cliente.nombre);

    char continuar;
    do {
        limpiarPantalla();
        mostrarProductosDisponibles();

        int idProducto, cantidad;
        cout << " " << endl;
        cout << "-------------------------------------------" << endl;
        cout << "Ingrese el ID del producto que desea comprar: ";
        cin >> idProducto;
        cout << "-------------------------------------------" << endl;
        cout << "Ingrese la cantidad: ";
        cin >> cantidad;

        if (idProducto < 1 || idProducto > productosDisponibles.size()) {
            cout << "-------------------------------------------" << endl;
            cout << "ID de producto no valido. Intente de nuevo." << endl;
            continue;
        }

        if (cantidad > productosDisponibles[idProducto - 1].existencia) {
            cout << "-----------------------------------------------" << endl;
            cout << "No hay suficiente existencia. Intente de nuevo." << endl;
            continue;
        }

        Producto productoSeleccionado = productosDisponibles[idProducto - 1];
        productoSeleccionado.cantidad = cantidad;
        factura.productos.push_back(productoSeleccionado);

        productosDisponibles[idProducto - 1].existencia -= cantidad;

        cout << "-------------------------------------------" << endl;
        cout << "Desea agregar otro producto? (s/n): ";
        cin >> continuar;
        cin.ignore();
    } while (continuar == 's' || continuar == 'S');

    calcularTotales(factura);

    if (factura.total > 2499) {
        cout << "---------------------------------------------------------" << endl;
        cout << "El monto total excede Q2499. Se requiere NIT del cliente." << endl;
        cout << "Ingrese el NIT del cliente: ";
        getline(cin, factura.cliente.nit);
        factura.requiereNIT = true;
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
    }

    limpiarPantalla();
    menuFacturacion(factura);
}

void buscarFactura() {
    string codigo;
    cout << "Ingrese el codigo de la factura: ";
    getline(cin, codigo);

    for (const auto& factura : facturas) {
        if (factura.codigo == codigo) {
            mostrarFactura(factura);
            return;
        }
    }

    cout << "Factura no encontrada." << endl;
}

void mostrarTodasFacturas() {
    int opcion;
    cout << "\n=== ORDENAR FACTURAS POR ===" << endl;
    cout << "1. Codigo de factura" << endl;
    cout << "2. Nombre del cliente" << endl;
    cout << "3. Total de la factura" << endl;
    cout << "Seleccione una opcion: ";
    cin >> opcion;
    cin.ignore();

    switch (opcion) {
    case 1:
        sort(facturas.begin(), facturas.end(), compararPorCodigo);
        break;
    case 2:
        sort(facturas.begin(), facturas.end(), compararPorNombreCliente);
        break;
    case 3:
        sort(facturas.begin(), facturas.end(), compararPorTotal);
        break;
    default:
        cout << "Opcion no valida. Mostrando sin ordenar." << endl;
        break;
    }

    for (const auto& factura : facturas) {
        mostrarFactura(factura);
    }
}

void mostrarEstadoCuenta() {
    double totalVentas = 0;
    cout << " " << endl;
    cout << "-------------------------------------------" << endl;
    cout << "=== ESTADO DE CUENTA ===" << endl;
    for (const auto& factura : facturas) {
        totalVentas += factura.total;
        cout << "Factura: " << factura.codigo << " - Total: Q" << factura.total << endl;
    }
    cout << "Total de ventas: Q" << totalVentas << endl;
}

void agregarProducto() {
    limpiarPantalla();
    Producto nuevoProducto;

    cout << "=== AGREGAR NUEVO PRODUCTO ===" << endl;
    cout << "Ingrese los siguientes datos:" << endl;

    cout << "Nombre: ";
    getline(cin, nuevoProducto.nombre);

    cout << "Descripcion: ";
    getline(cin, nuevoProducto.descripcion);

    cout << "Precio: ";
    cin >> nuevoProducto.precio;

    cout << "Existencia inicial: ";
    cin >> nuevoProducto.existencia;
    cin.ignore(); // Limpiar el buffer de entrada

    // Inicializar cantidad en 0
    nuevoProducto.cantidad = 0;

    // Agregar el producto al vector
    productosDisponibles.push_back(nuevoProducto);

    // Guardar los cambios en el archivo JSON
    guardarDatos();

    cout << "\nProducto agregado exitosamente!" << endl;
}

void menu() {
    cargarDatos(); // Cargar datos al iniciar el programa

    int opcion;
    do {
        limpiarPantalla();
        cout << "=== MENU PRINCIPAL ===" << endl;
        cout << "1. Emitir Factura" << endl;
        cout << "2. Buscar Factura" << endl;
        cout << "3. Mostrar Todas las Facturas" << endl;
        cout << "4. Mostrar Estado de Cuenta" << endl;
        cout << "5. Mostrar Inventario" << endl;
        cout << "6. Agregar productos al inventario" << endl;
        cout << "7. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1:
            emitirFactura();
            break;
        case 2:
            buscarFactura();
            break;
        case 3:
            mostrarTodasFacturas();
            break;
        case 4:
            mostrarEstadoCuenta();
            break;
        case 5:
            mostrarProductosDisponibles();
            break;
        case 6:
            agregarProducto();
            break;
        case 7:
            guardarDatos(); // Guardar datos antes de salir
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opcion no valida. Intente de nuevo." << endl;
        }
        cout << "Presione Enter para continuar...";
        cin.get();
    } while (opcion != 7);
}

int main() {
    menu();
    return 0;
}
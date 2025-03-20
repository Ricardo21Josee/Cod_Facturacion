#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib> // Para system("cls") o system("clear")

using namespace std;

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

// Productos predefinidos
vector<Producto> productosDisponibles = {
    {"Laptop HP", "Laptop HP Pavilion 15.6, 8GB RAM, 512GB SSD", 5500.00, 0, 5},
    {"iPhone 14", "Smartphone Apple iPhone 14, 128GB, color Midnight", 7500.00, 0, 5},
    {"Samsung TV", "Televisor Samsung 55 pulgadas 4K UHD Smart TV", 4500.00, 0, 5},
    {"Tablet Lenovo", "Tablet Lenovo Tab M10, 10.1 pulgadas, 64GB", 1200.00, 0, 5},
    {"Monitor Dell", "Monitor Dell 27 pulgadas, Full HD, 75Hz", 1800.00, 0, 5},
    {"Teclado Logitech", "Teclado inalámbrico Logitech K380", 250.00, 0, 5},
    {"Mouse Razer", "Mouse gaming Razer DeathAdder V2", 400.00, 0, 5},
    {"Impresora Epson", "Impresora multifuncional Epson EcoTank L3150", 2200.00, 0, 5},
    {"Disco Duro SSD", "Disco duro SSD Kingston 1TB", 800.00, 0, 5},
    {"Router TP-Link", "Router Wi-Fi TP-Link Archer C6", 350.00, 0, 5}
};

vector<Factura> facturas;

// Funciones
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mostrarProductosDisponibles() {
    cout << "\n=== PRODUCTOS DISPONIBLES ===" << endl;
    cout << setw(5) << left << "ID"
        << setw(20) << left << "Nombre"
        << setw(50) << left << "Descripción"
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
    cout << "Dirección: " << factura.direccionEmpresa << endl;
    cout << "NIT: " << factura.nitEmpresa << endl;
    cout << "Código de Factura: " << factura.codigo << endl;
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
        cout << "\n=== OPCIONES DE FACTURACIÓN ===" << endl;
        cout << "1. Agregar otro producto" << endl;
        cout << "2. Eliminar un producto" << endl;
        cout << "3. Emitir factura" << endl;
        cout << "4. Cancelar factura" << endl;
        cout << "Seleccione una opción: ";
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
                cout << "ID de producto no válido. Intente de nuevo." << endl;
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
            cout << "Ingrese el número del producto que desea eliminar: ";
            int idEliminar;
            cin >> idEliminar;

            if (idEliminar < 1 || idEliminar > factura.productos.size()) {
                cout << "------------------------------------------------" << endl;
                cout << "Número de producto no válido." << endl;
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
            cout << "--------------------------------------" << endl;
            cout << "Factura emitida con código: " << factura.codigo << endl;
            return;
        }
        case 4: {
            devolverProductosAlInventario(factura);
            cout << "------------------------------------------------------------------" << endl;
            cout << "Factura cancelada. Los productos han sido devueltos al inventario." << endl;
            return;
        }
        default:
            cout << "-----------------------------------" << endl;
            cout << "Opción no válida. Intente de nuevo." << endl;

        cout << " " << endl;
        }cout << "-------------------------------" << endl;
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
            cout << "ID de producto no válido. Intente de nuevo." << endl;
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
        cout << "¿Desea agregar otro producto? (s/n): ";
        cin >> continuar;
        cin.ignore();
    } while (continuar == 's' || continuar == 'S');

    calcularTotales(factura);

    if (factura.requiereNIT) {
        cout << "---------------------------------------------------------" << endl;
        cout << "El monto total excede Q2499. Se requiere NIT del cliente." << endl;
        cout << "Ingrese el NIT del cliente: ";
        getline(cin, factura.cliente.nit);
    }

    limpiarPantalla();
    menuFacturacion(factura);
}

void buscarFactura() {
    string codigo;
    cout << "Ingrese el código de la factura: ";
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

void menu() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "=== MENÚ PRINCIPAL ===" << endl;
        cout << "1. Emitir Factura" << endl;
        cout << "2. Buscar Factura" << endl;
        cout << "3. Mostrar Todas las Facturas" << endl;
        cout << "4. Mostrar Estado de Cuenta" << endl;
        cout << "5. Mostrar Inventario" << endl;
        cout << "6. Salir" << endl;
        cout << "Seleccione una opción: ";
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
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opción no válida. Intente de nuevo." << endl;
        }
        cout << "Presione Enter para continuar...";
        cin.get();
    } while (opcion != 6);
}

int main() {
    menu();
    return 0;
}
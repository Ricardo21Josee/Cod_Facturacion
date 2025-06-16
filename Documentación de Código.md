<h1 align="center"><b>Sistema de Facturación y Control de Inventario con PostgreSQL</b><img src="https://media.giphy.com/media/hvRJCLFzcasrR4ia7z/giphy.gif" width="35"></h1>
<br>


<div class="section">
    <h2>Descripción General</h2>
    <p>Este sistema de facturación y control de inventario está desarrollado en C++ moderno (C++17) y utiliza PostgreSQL como sistema de gestión de base de datos. Está diseñado para Tienda Facturación DB, proporcionando gestión completa de inventario, generación de facturas y seguimiento de clientes.</p>
    
<h3>Características Principales</h3>
    <ul>
        <li>Conexión robusta a base de datos PostgreSQL</li>
        <li>Gestión completa de inventario</li>
        <li>Generación de facturas con cálculo automático de IVA (12%)</li>
        <li>Diferencia entre clientes con NIT y consumidores finales</li>
        <li>Validación de datos y manejo de errores</li>
        <li>Interfaz de consola intuitiva</li>
    </ul>
</div>

<div class="section">
    <h2>Estructuras de Datos</h2>
    
<h3>Estructura Producto</h3>
    <pre><code>struct Producto {
    int id = 0;
    string nombre = "";
    string descripcion = "";
    double precio = 0.0;
    int existencia = 0;
};</code></pre>
    <table>
        <tr>
            <th>Campo</th>
            <th>Tipo</th>
            <th>Descripción</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Identificador único del producto</td>
        </tr>
        <tr>
            <td>nombre</td>
            <td>string</td>
            <td>Nombre del producto</td>
        </tr>
        <tr>
            <td>descripcion</td>
            <td>string</td>
            <td>Descripción detallada del producto</td>
        </tr>
        <tr>
            <td>precio</td>
            <td>double</td>
            <td>Precio unitario del producto</td>
        </tr>
        <tr>
            <td>existencia</td>
            <td>int</td>
            <td>Cantidad disponible en inventario</td>
        </tr>
    </table>

<h3>Estructura Cliente</h3>
    <pre><code>struct Cliente {
    int id = 0;
    string nombre = "";
    string apellido = "";
    string direccion = "";
    string dpi = "";
    string nit = "";
};</code></pre>
    <table>
        <tr>
            <th>Campo</th>
            <th>Tipo</th>
            <th>Descripción</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Identificador único del cliente</td>
        </tr>
        <tr>
            <td>nombre</td>
            <td>string</td>
            <td>Nombre del cliente</td>
        </tr>
        <tr>
            <td>apellido</td>
            <td>string</td>
            <td>Apellido del cliente</td>
        </tr>
        <tr>
            <td>direccion</td>
            <td>string</td>
            <td>Dirección del cliente</td>
        </tr>
        <tr>
            <td>dpi</td>
            <td>string</td>
            <td>Documento Personal de Identificación (13 dígitos)</td>
        </tr>
        <tr>
            <td>nit</td>
            <td>string</td>
            <td>Número de Identificación Tributaria</td>
        </tr>
    </table>

<h3>Estructura ClienteCF (Consumidor Final)</h3>
    <pre><code>struct ClienteCF {
    int id = 0;
    string nombre = "";
    string direccion = "";
};</code></pre>
    <table>
        <tr>
            <th>Campo</th>
            <th>Tipo</th>
            <th>Descripción</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Identificador único del consumidor final</td>
        </tr>
        <tr>
            <td>nombre</td>
            <td>string</td>
            <td>Nombre del consumidor final</td>
        </tr>
        <tr>
            <td>direccion</td>
            <td>string</td>
            <td>Dirección del consumidor final</td>
        </tr>
    </table>

<h3>Estructura Factura</h3>
    <pre><code>struct Factura {
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
    vector&lt;Producto&gt; productos;
};</code></pre>
    <table>
        <tr>
            <th>Campo</th>
            <th>Tipo</th>
            <th>Descripción</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Identificador único de la factura</td>
        </tr>
        <tr>
            <td>codigo</td>
            <td>string</td>
            <td>Código único de factura generado automáticamente</td>
        </tr>
        <tr>
            <td>nombreEmpresa</td>
            <td>string</td>
            <td>Nombre de la empresa emisora</td>
        </tr>
        <tr>
            <td>direccionEmpresa</td>
            <td>string</td>
            <td>Dirección de la empresa emisora</td>
        </tr>
        <tr>
            <td>nitEmpresa</td>
            <td>string</td>
            <td>NIT de la empresa emisora</td>
        </tr>
        <tr>
            <td>cliente_id</td>
            <td>int</td>
            <td>ID del cliente (si aplica)</td>
        </tr>
        <tr>
            <td>cf_cliente_id</td>
            <td>int</td>
            <td>ID del consumidor final (si aplica)</td>
        </tr>
        <tr>
            <td>subtotal_sin_iva</td>
            <td>double</td>
            <td>Subtotal antes de aplicar IVA</td>
        </tr>
        <tr>
            <td>subtotal</td>
            <td>double</td>
            <td>Subtotal con IVA incluido</td>
        </tr>
        <tr>
            <td>iva</td>
            <td>double</td>
            <td>Monto del IVA calculado (12%)</td>
        </tr>
        <tr>
            <td>total</td>
            <td>double</td>
            <td>Total a pagar</td>
        </tr>
        <tr>
            <td>requiere_nit</td>
            <td>bool</td>
            <td>Indica si la factura requiere NIT</td>
        </tr>
        <tr>
            <td>productos</td>
            <td>vector&lt;Producto&gt;</td>
            <td>Lista de productos incluidos en la factura</td>
        </tr>
    </table>
</div>

<div class="section">
    <h2>Funciones Principales</h2>
    
<h3>Conexión a la Base de Datos</h3>
    <h4>conectarDB()</h4>
    <p>Establece conexión con la base de datos PostgreSQL:</p>
    <ul>
        <li>Utiliza parámetros de conexión configurados</li>
        <li>Maneja errores de conexión</li>
        <li>Retorna un objeto connection de pqxx</li>
    </ul>
    
<h4>verificarTablas(connection& C)</h4>
    <p>Verifica que todas las tablas necesarias existan en la base de datos:</p>
    <ul>
        <li>productos_inventario</li>
        <li>clientes</li>
        <li>clientes_cf</li>
        <li>facturas</li>
        <li>pre_detalle_factura</li>
    </ul>
    
<h3>Gestión de Inventario</h3>
    <h4>obtenerInventario(connection& C)</h4>
    <p>Recupera todos los productos del inventario ordenados por ID:</p>
    <ul>
        <li>Ejecuta consulta SQL</li>
        <li>Retorna vector de Producto</li>
    </ul>
    
<h4>obtenerProductoPorId(connection& C, int id)</h4>
    <p>Obtiene un producto específico por su ID:</p>
    <ul>
        <li>Lanza excepción si no encuentra el producto</li>
        <li>Retorna objeto Producto</li>
    </ul>
    
<h4>actualizarExistencias(connection& C, int producto_id, int nueva_existencia)</h4>
    <p>Actualiza la cantidad en existencia de un producto:</p>
    <ul>
        <li>Retorna true si la actualización fue exitosa</li>
    </ul>
    
<h4>reducirExistencias(connection& C, int producto_id, int cantidad)</h4>
    <p>Reduce la existencia de un producto al venderlo:</p>
    <ul>
        <li>Verifica que haya suficiente existencia</li>
        <li>Retorna true si la operación fue exitosa</li>
    </ul>
    
 <h4>agregarProducto(connection& C, const Producto& p)</h4>
    <p>Agrega un nuevo producto al inventario:</p>
    <ul>
        <li>Retorna true si la inserción fue exitosa</li>
    </ul>
    
<h3>Facturación</h3>
    <h4>guardarFactura(connection& C, Factura& factura, const string& nombreCliente, const string& nit, const string& direccion, const string& dpi)</h4>
    <p>Guarda una factura completa en la base de datos:</p>
    <ul>
        <li>Inserta cliente según si tiene NIT o es consumidor final</li>
        <li>Genera registro de factura</li>
        <li>Guarda detalles de productos facturados</li>
        <li>Retorna true si la operación fue exitosa</li>
    </ul>
    
<h3>Interfaz de Usuario</h3>
    <h4>menuInventario(connection& C)</h4>
    <p>Muestra menú de gestión de inventario con opciones para:</p>
    <ul>
        <li>Ver inventario completo</li>
        <li>Buscar producto por ID</li>
        <li>Agregar nuevo producto</li>
        <li>Actualizar existencias</li>
    </ul>
    
<h4>emitirFactura(connection& C)</h4>
    <p>Guía al usuario a través del proceso de facturación:</p>
    <ul>
        <li>Captura datos del cliente</li>
        <li>Permite seleccionar productos y cantidades</li>
        <li>Calcula totales automáticamente</li>
        <li>Genera código único de factura</li>
        <li>Muestra resumen antes de guardar</li>
    </ul>
    
<h4>menuPrincipal(connection& C)</h4>
    <p>Muestra el menú principal con opciones para:</p>
    <ul>
        <li>Gestión de inventario</li>
        <li>Emisión de facturas</li>
        <li>Salir del sistema</li>
    </ul>
</div>

<div class="section">
    <h2>Configuración Requerida</h2>
    
<h3>Requisitos del Sistema</h3>
    <ul>
        <li>Compilador C++17 (g++ 7+, clang 5+, MSVC 2017+)</li>
        <li>PostgreSQL 9.5+</li>
        <li>Biblioteca libpqxx 6.0+</li>
    </ul>
    
<h3>Configuración de la Base de Datos</h3>
    <p>Modificar las constantes al inicio del código según su entorno:</p>
    <pre><code>const string DB_NAME = "Tienda_Facturacion_DB";
const string DB_USER = "ricardo";
const string DB_PASSWORD = "ricgio921";
const string DB_HOST = "localhost";
const string DB_PORT = "5432";</code></pre>
    
<h3>Estructura de la Base de Datos</h3>
    <p>El sistema espera las siguientes tablas:</p>
    <ul>
        <li><strong>productos_inventario</strong>: Almacena información de productos</li>
        <li><strong>clientes</strong>: Almacena clientes con NIT</li>
        <li><strong>clientes_cf</strong>: Almacena consumidores finales</li>
        <li><strong>facturas</strong>: Registro de facturas emitidas</li>
        <li><strong>pre_detalle_factura</strong>: Detalle de productos por factura</li>
    </ul>
</div>

<div class="section">
    <h2>Guía de Uso</h2>
    
<h3>Compilación y Ejecución</h3>
    <ol>
        <li>Compilar con soporte para C++17:
            <pre>g++ -std=c++17 -o sistema_facturacion main.cpp -lpqxx -lpq</pre>
        </li>
        <li>Ejecutar el programa:
            <pre>./sistema_facturacion</pre>
        </li>
    </ol>
    
<h3>Flujo de Trabajo</h3>
    <h4>1. Gestión de Inventario</h4>
    <ol>
        <li>Seleccionar "Gestión de Inventario" del menú principal</li>
        <li>Agregar productos con nombre, descripción, precio y existencia inicial</li>
        <li>Actualizar existencias según ventas o reposiciones</li>
    </ol>
    
<h4>2. Emisión de Facturas</h4>
    <ol>
        <li>Seleccionar "Emitir Factura" del menú principal</li>
        <li>Ingresar datos del cliente (nombre, dirección)</li>
        <li>Proporcionar NIT si es requerido (facturas mayores a Q2499)</li>
        <li>Seleccionar productos y cantidades</li>
        <li>Revisar resumen y confirmar</li>
    </ol>
    
<h3>Ejemplo de Facturación</h3>
    <pre>
1. Seleccionar "2. Emitir Factura"
2. Ingresar nombre: "María López"
3. Ingresar dirección: "5ta avenida 8-90 zona 1"
4. Ingresar NIT: "98765432-1" (requerido para facturas > Q2499)
5. Ingresar DPI: "1234567890123" (13 dígitos)
6. Seleccionar productos:
   - ID 1: Laptop (Q5000) x 2 unidades
   - ID 3: Mouse (Q150) x 1 unidad
7. Confirmar factura
8. Sistema muestra resumen con:
   - Subtotal: Q10150
   - IVA (12%): Q1089.29
   - Total: Q10150
9. Factura se guarda con código único (ej: FAC-1623456789-456)</pre>
</div>

<div class="section">
    <h2>Consideraciones Importantes</h2>
    
   <div class="warning">
        <h4>Cumplimiento Tributario</h4>
        <p>El sistema valida que facturas superiores a Q2499 incluyan NIT del cliente, cumpliendo con las regulaciones fiscales de Guatemala.</p>
    </div>
    
<div class="note">
        <h4>Seguridad de Datos</h4>
        <p>Toda la información se almacena en PostgreSQL, proporcionando:</p>
        <ul>
            <li>Integridad de datos</li>
            <li>Transacciones ACID</li>
            <li>Respaldo y recuperación</li>
        </ul>
    </div>
    
<h3>Recomendaciones</h3>
    <ul>
        <li>Realizar respaldos periódicos de la base de datos</li>
        <li>Configurar usuarios con los permisos adecuados</li>
        <li>Implementar red privada para la conexión a la base de datos en entornos productivos</li>
    </ul>
</div>

<div class="section">
    <h2>Posibles Mejoras</h2>
    <ul>
        <li>Implementar autenticación de usuarios</li>
        <li>Agregar reportes detallados de ventas</li>
        <li>Implementar búsqueda avanzada de facturas</li>
        <li>Desarrollar interfaz gráfica</li>
        <li>Agregar soporte para múltiples sucursales</li>
    </ul>
</div>

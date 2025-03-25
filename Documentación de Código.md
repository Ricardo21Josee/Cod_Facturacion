<h1 align="center"><b>Sistema de Facturación y Control de Inventario con almacenamiento en JSON</b><img src="https://media.giphy.com/media/hvRJCLFzcasrR4ia7z/giphy.gif" width="35"></h1>
    <br>
    <h1>Acerca del Código</h1>
    <p>Este programa en C++ es un <strong>Sistema de Punto de Venta (POS)</strong> diseñado para TecnoShop GT, un negocio minorista en Guatemala. Proporciona capacidades completas de gestión de inventario, facturación y reportes de ventas con persistencia de datos en JSON.</p>

  <div class="section">
        <h2>Visión General del Sistema</h2>
        <p>El sistema POS consta de varios componentes clave:</p>
        <ul>
            <li>Gestión de inventario de productos</li>
            <li>Generación de facturas con cálculos de impuestos</li>
            <li>Manejo de información de clientes</li>
            <li>Reportes de ventas y análisis</li>
            <li>Persistencia de datos usando archivos JSON</li>
        </ul>
        
  <h3>Características Principales</h3>
        <ul>
            <li>Cálculo automático de IVA (12%)</li>
            <li>Aplicación de requisito de NIT para facturas mayores a Q2499</li>
            <li>Seguimiento de inventario con actualización de stock</li>
            <li>Múltiples opciones de ordenamiento de facturas</li>
            <li>Reportes resumidos de ventas</li>
            <li>Compatibilidad multiplataforma (Windows/Linux)</li>
        </ul>
    </div>
<div class="section">
        <h2>Estructuras de Datos</h2>
        
  <h3>Estructura de Producto</h3>
        <div class="code-header">Estructura Producto</div>
        <pre class="code-block"><code>struct Producto {
    string nombre;
    string descripcion;
    double precio = 0;
    int cantidad = 0;
    int existencia = 0;
};</code></pre>
        <table>
            <tr>
                <th>Campo</th>
                <th>Tipo</th>
                <th>Descripción</th>
            </tr>
            <tr>
                <td>nombre</td>
                <td>string</td>
                <td>Nombre del producto</td>
            </tr>
            <tr>
                <td>descripcion</td>
                <td>string</td>
                <td>Descripción del producto</td>
            </tr>
            <tr>
                <td>precio</td>
                <td>double</td>
                <td>Precio unitario del producto</td>
            </tr>
            <tr>
                <td>cantidad</td>
                <td>int</td>
                <td>Cantidad en la transacción actual (valor por defecto: 0)</td>
            </tr>
            <tr>
                <td>existencia</td>
                <td>int</td>
                <td>Cantidad actual en stock (valor por defecto: 0)</td>
            </tr>
        </table>

  <h3>Estructura de Cliente</h3>
        <div class="code-header">Estructura Cliente</div>
        <pre class="code-block"><code>struct Cliente {
    string nombre;
    string nit;
};</code></pre>
        <table>
            <tr>
                <th>Campo</th>
                <th>Tipo</th>
                <th>Descripción</th>
            </tr>
            <tr>
                <td>nombre</td>
                <td>string</td>
                <td>Nombre del cliente</td>
            </tr>
            <tr>
                <td>nit</td>
                <td>string</td>
                <td>Número de identificación tributaria (NIT) del cliente</td>
            </tr>
        </table>

  <h3>Estructura de Factura</h3>
        <div class="code-header">Estructura Factura</div>
        <pre class="code-block"><code>struct Factura {
    string codigo;
    string nombreEmpresa;
    string direccionEmpresa;
    string nitEmpresa;
    Cliente cliente;
    vector&lt;Producto&gt; productos;
    double siva = 0;
    double subtotal = 0;
    double iva = 0;
    double total = 0;
    bool requiereNIT = false;
};</code></pre>
        <table>
            <tr>
                <th>Campo</th>
                <th>Tipo</th>
                <th>Descripción</th>
            </tr>
            <tr>
                <td>codigo</td>
                <td>string</td>
                <td>Código único de factura (ej. "FAC-1")</td>
            </tr>
            <tr>
                <td>nombreEmpresa</td>
                <td>string</td>
                <td>Nombre de la empresa (constante: "TecnoShop GT")</td>
            </tr>
            <tr>
                <td>direccionEmpresa</td>
                <td>string</td>
                <td>Dirección de la empresa (constante)</td>
            </tr>
            <tr>
                <td>nitEmpresa</td>
                <td>string</td>
                <td>NIT de la empresa (constante: "123456-7")</td>
            </tr>
            <tr>
                <td>cliente</td>
                <td>Cliente</td>
                <td>Estructura con información del cliente</td>
            </tr>
            <tr>
                <td>productos</td>
                <td>vector&lt;Producto&gt;</td>
                <td>Lista de productos en la factura</td>
            </tr>
            <tr>
                <td>siva</td>
                <td>double</td>
                <td>Subtotal sin IVA (calculado)</td>
            </tr>
            <tr>
                <td>subtotal</td>
                <td>double</td>
                <td>Total antes de IVA (calculado)</td>
            </tr>
            <tr>
                <td>iva</td>
                <td>double</td>
                <td>Monto de IVA (12% de siva)</td>
            </tr>
            <tr>
                <td>total</td>
                <td>double</td>
                <td>Monto final a pagar (subtotal + iva)</td>
            </tr>
            <tr>
                <td>requiereNIT</td>
                <td>bool</td>
                <td>Indicador de requisito de NIT (true si total > 2499)</td>
            </tr>
        </table>
    </div>

  <div class="section">
        <h2>Funciones Principales</h2>
        
  <h3>Persistencia de Datos</h3>
        <p>El sistema usa archivos JSON para almacenamiento de datos con la biblioteca nlohmann/json.</p>
        
  <h4>guardarDatos()</h4>
        <p>Guarda todos los datos del sistema en el archivo "datos.json":</p>
        <ul>
            <li>Convierte datos de productos y facturas a formato JSON</li>
            <li>Crea un objeto JSON estructurado con arrays para productos y facturas</li>
            <li>Escribe en el archivo con formato legible (indentación de 4 espacios)</li>
        </ul>
        
  <h4>cargarDatos()</h4>
        <p>Carga los datos del sistema desde "datos.json":</p>
        <ul>
            <li>Crea un nuevo archivo vacío si no existe</li>
            <li>Interpreta los datos JSON en estructuras de C++</li>
            <li>Llena los vectores globales productosDisponibles y facturas</li>
        </ul>
        
  <h3>Funciones de Utilidad</h3>
        
  <h4>limpiarPantalla()</h4>
        <p>Limpia la pantalla de la consola con comandos específicos para cada plataforma:</p>
        <ul>
            <li>Usa "cls" en Windows</li>
            <li>Usa "clear" en sistemas tipo Unix</li>
        </ul>
        
  <h4>calcularTotales(Factura& factura)</h4>
        <p>Calcula los valores financieros de la factura:</p>
        <ul>
            <li>Subtotal: Suma de todos los precios de productos × cantidades</li>
            <li>siva: Subtotal dividido por 1.12 (monto antes de IVA)</li>
            <li>iva: 12% de siva</li>
            <li>total: siva + iva</li>
            <li>Establece el flag requiereNIT si el total excede 2499</li>
        </ul>
        
  <h4>devolverProductosAlInventario(Factura& factura)</h4>
        <p>Devuelve los productos de una factura cancelada al inventario:</p>
        <ul>
            <li>Empareja productos por nombre</li>
            <li>Incrementa el campo existencia por cada producto devuelto</li>
        </ul>
    </div>

  <div class="section">
        <h2>Componentes de la Interfaz de Usuario</h2>
        
  <h3>Menú Principal</h3>
        <p>Punto de entrada de la aplicación con estas opciones:</p>
        <ol>
            <li><strong>Emitir Factura</strong>: Crear una nueva factura</li>
            <li><strong>Buscar Factura</strong>: Buscar una factura existente</li>
            <li><strong>Mostrar Todas las Facturas</strong>: Mostrar todas las facturas con opciones de ordenamiento</li>
            <li><strong>Mostrar Estado de Cuenta</strong>: Mostrar resumen de ventas</li>
            <li><strong>Mostrar Inventario</strong>: Mostrar inventario actual de productos</li>
            <li><strong>Agregar productos al inventario</strong>: Añadir nuevos productos</li>
            <li><strong>Salir</strong>: Salir de la aplicación</li>
        </ol>
        
  <h3>Flujo de Creación de Facturas</h3>
        <ol>
            <li>Ingresar nombre del cliente</li>
            <li>Añadir productos por ID con cantidades</li>
            <li>El sistema verifica disponibilidad en stock</li>
            <li>Opción para añadir más productos</li>
            <li>Verificación automática de requisito de NIT para totales > 2499</li>
            <li>Menú de edición de factura con opciones para:
                <ul>
                    <li>Añadir más productos</li>
                    <li>Eliminar productos</li>
                    <li>Finalizar factura</li>
                    <li>Cancelar factura (devuelve productos al inventario)</li>
                </ul>
            </li>
        </ol>
        
  <h3>Funciones de Visualización</h3>
        
  <h4>mostrarProductosDisponibles()</h4>
        <p>Muestra los productos disponibles en una tabla formateada con columnas:</p>
        <ul>
            <li>ID (índice basado en 1)</li>
            <li>Nombre</li>
            <li>Descripción</li>
            <li>Precio</li>
            <li>Stock (existencia)</li>
        </ul>
        
  <h4>mostrarFactura(const Factura& factura)</h4>
        <p>Muestra información completa de la factura incluyendo:</p>
        <ul>
            <li>Información de la empresa</li>
            <li>Detalles del cliente</li>
            <li>Lista de productos con cantidades y totales</li>
            <li>Desglose financiero (subtotales, IVA, total)</li>
            <li>Indicación de NIT/CF</li>
        </ul>
    </div>

<div class="section">
        <h1 align="center">Guía de Uso</h1>
        
  <h3>Primeros Pasos</h3>
        <ol>
            <li>Compile el programa con un compilador compatible con C++17</li>
            <li>Asegúrese que nlohmann/json.hpp esté en su ruta de includes</li>
            <li>Ejecute el programa</li>
            <li>El sistema creará un archivo "datos.json" si no existe</li>
        </ol>
        
  <h3>Añadir Productos</h3>
        <p>Desde el menú principal, seleccione la opción 6 y proporcione:</p>
        <ul>
            <li>Nombre del producto</li>
            <li>Descripción</li>
            <li>Precio</li>
            <li>Cantidad inicial en stock</li>
        </ul>
        
  <h3>Crear una Factura</h3>
        <ol>
            <li>Seleccione "Emitir Factura" del menú principal</li>
            <li>Ingrese el nombre del cliente</li>
            <li>Añada productos por ID y cantidad</li>
            <li>Continúe añadiendo productos o finalice</li>
            <li>Proporcione NIT si es requerido o solicitado</li>
            <li>Use el menú de factura para finalizar o hacer cambios</li>
        </ol>
        
  <h3>Ejemplo de Creación de Factura</h3>
        <pre>
1. Seleccione "1. Emitir Factura"
2. Ingrese nombre del cliente: "Juan Pérez"
3. Vea la lista de productos y note los IDs:
   - 1 Laptop  Q5000 (10 en stock)
   - 2 Mouse   Q150 (25 en stock)
4. Añada producto 1, cantidad 2
5. Añada producto 2, cantidad 1
6. Elija no añadir más productos
7. El sistema calcula el total (Q10180 + 12% IVA)
8. Como total > Q2499, ingrese NIT: "12345678-9"
9. Finalice la factura
        </pre>
        
  <h3>Visualización de Reportes</h3>
        <p><strong>Todas las Facturas:</strong> Opción 3 con ordenamiento por:
        <ul>
            <li>Código de factura</li>
            <li>Nombre del cliente</li>
            <li>Monto total</li>
        </ul>
        
  <p><strong>Resumen de Ventas:</strong> Opción 4 muestra:
        <ul>
            <li>Lista de todas las facturas con sus totales</li>
            <li>Total general de todas las ventas</li>
        </ul>
    </div>

  <div class="section">
        <h2>Notas Importantes</h2>
        
  <div class="note">
            <h4>Persistencia de Datos</h4>
            <p>El sistema guarda automáticamente los datos en "datos.json" cuando:</p>
            <ul>
                <li>Se crea o cancela una factura</li>
                <li>Se añaden nuevos productos</li>
                <li>Se sale del programa</li>
            </ul>
        </div>
        
   <div class="warning">
            <h4>Cumplimiento Tributario</h4>
            <p>El sistema aplica la recolección de NIT para facturas mayores a Q2499 según lo requiere la ley tributaria guatemalteca. Para facturas menores, los usuarios pueden elegir entre NIT o CF (Consumidor Final).</p>
        </div>
        
  <h3>Consideraciones Técnicas</h3>
        <ul>
            <li>El emparejamiento de productos se hace por nombre - asegure nombres únicos</li>
            <li>Los códigos de factura son secuenciales (FAC-1, FAC-2, etc.)</li>
            <li>El sistema usa la tasa de IVA guatemalteca del 12%</li>
            <li>Todos los valores monetarios están en Quetzales (Q)</li>
        </ul>
    </div>

  <div class="section">
        <h2>Conclusión</h2>
        <p>
            Futuras mejoras podrían incluir soporte para escaneo de códigos de barras, autenticación de usuarios, y características de reportes más detallados.
        </p>
    </div>

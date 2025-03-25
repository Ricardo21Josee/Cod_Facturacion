<h1 align="center"><b>Billing and inventory control system code utilizing JSON file storage</b><img src="https://media.giphy.com/media/hvRJCLFzcasrR4ia7z/giphy.gif" width="35"></h1>
<br>



 <h1>About the Code</h1>
 <p>This C++ program is a <strong>Retail Point of Sale (POS) System</strong> designed for TecnoShop GT, a retail business in Guatemala. It provides comprehensive inventory management, invoicing, and sales reporting capabilities with JSON data persistence.</p>




  <div class="section">
        <h2>System Overview</h2>
        <p>
            The POS system consists of several key components:
        </p>
        <ul>
            <li>Product inventory management</li>
            <li>Invoice generation with tax calculations</li>
            <li>Customer information handling</li>
            <li>Sales reporting and analytics</li>
            <li>Data persistence using JSON files</li>
        </ul>
        
  <h3>Key Features</h3>
        <ul>
            <li>Automatic VAT (IVA) calculation (12%)</li>
            <li>NIT requirement enforcement for invoices over Q2499</li>
            <li>Inventory tracking with stock updates</li>
            <li>Multiple invoice sorting options</li>
            <li>Sales summary reporting</li>
            <li>Cross-platform compatibility (Windows/Linux)</li>
        </ul>
    </div>

  <div class="section">
        <h2>Data Structures</h2>
        
  <h3>Product Structure</h3>
        <pre><code>struct Producto {
    string nombre;
    string descripcion;
    double precio = 0;
    int cantidad = 0;
    int existencia = 0;
};</code></pre>
        <table>
            <tr>
                <th>Field</th>
                <th>Type</th>
                <th>Description</th>
            </tr>
            <tr>
                <td>nombre</td>
                <td>string</td>
                <td>Name of the product</td>
            </tr>
            <tr>
                <td>descripcion</td>
                <td>string</td>
                <td>Product description</td>
            </tr>
            <tr>
                <td>precio</td>
                <td>double</td>
                <td>Unit price of the product</td>
            </tr>
            <tr>
                <td>cantidad</td>
                <td>int</td>
                <td>Quantity in current transaction (default: 0)</td>
            </tr>
            <tr>
                <td>existencia</td>
                <td>int</td>
                <td>Current stock quantity (default: 0)</td>
            </tr>
        </table>

  <h3>Client Structure</h3>
        <pre><code>struct Cliente {
    string nombre;
    string nit;
};</code></pre>
        <table>
            <tr>
                <th>Field</th>
                <th>Type</th>
                <th>Description</th>
            </tr>
            <tr>
                <td>nombre</td>
                <td>string</td>
                <td>Customer name</td>
            </tr>
            <tr>
                <td>nit</td>
                <td>string</td>
                <td>Customer tax identification number (NIT)</td>
            </tr>
        </table>

  <h3>Invoice Structure</h3>
        <pre><code>struct Factura {
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
                <th>Field</th>
                <th>Type</th>
                <th>Description</th>
            </tr>
            <tr>
                <td>codigo</td>
                <td>string</td>
                <td>Unique invoice code (e.g., "FAC-1")</td>
            </tr>
            <tr>
                <td>nombreEmpresa</td>
                <td>string</td>
                <td>Business name (constant: "TecnoShop GT")</td>
            </tr>
            <tr>
                <td>direccionEmpresa</td>
                <td>string</td>
                <td>Business address (constant)</td>
            </tr>
            <tr>
                <td>nitEmpresa</td>
                <td>string</td>
                <td>Business tax ID (constant: "123456-7")</td>
            </tr>
            <tr>
                <td>cliente</td>
                <td>Cliente</td>
                <td>Customer information structure</td>
            </tr>
            <tr>
                <td>productos</td>
                <td>vector&lt;Producto&gt;</td>
                <td>List of products in the invoice</td>
            </tr>
            <tr>
                <td>siva</td>
                <td>double</td>
                <td>Subtotal without VAT (calculated)</td>
            </tr>
            <tr>
                <td>subtotal</td>
                <td>double</td>
                <td>Total before VAT (calculated)</td>
            </tr>
            <tr>
                <td>iva</td>
                <td>double</td>
                <td>VAT amount (12% of siva)</td>
            </tr>
            <tr>
                <td>total</td>
                <td>double</td>
                <td>Final amount to pay (subtotal + iva)</td>
            </tr>
            <tr>
                <td>requiereNIT</td>
                <td>bool</td>
                <td>Flag for NIT requirement (true if total > 2499)</td>
            </tr>
        </table>
    </div>

  <div class="section">
        <h2>Core Functions</h2>
        
  <h3>Data Persistence</h3>
        <p>The system uses JSON files for data storage with the nlohmann/json library.</p>
        
  <h4>guardarDatos()</h4>
        <p>Saves all system data to "datos.json" file:</p>
        <ul>
            <li>Converts product and invoice data to JSON format</li>
            <li>Creates a structured JSON object with arrays for products and invoices</li>
            <li>Writes to file with pretty printing (4-space indentation)</li>
        </ul>
        
  <h4>cargarDatos()</h4>
        <p>Loads system data from "datos.json":</p>
        <ul>
            <li>Creates a new empty file if none exists</li>
            <li>Parses JSON data into C++ structures</li>
            <li>Populates the global vectors productosDisponibles and facturas</li>
        </ul>
        
  <h3>Utility Functions</h3>
        
  <h4>limpiarPantalla()</h4>
        <p>Clears the console screen with platform-specific commands:</p>
        <ul>
            <li>Uses "cls" on Windows</li>
            <li>Uses "clear" on Unix-like systems</li>
        </ul>
        
  <h4>calcularTotales(Factura& factura)</h4>
        <p>Calculates invoice financial values:</p>
        <ul>
            <li>Subtotal: Sum of all product prices × quantities</li>
            <li>siva: Subtotal divided by 1.12 (amount before VAT)</li>
            <li>iva: 12% of siva</li>
            <li>total: siva + iva</li>
            <li>Sets requiereNIT flag if total exceeds 2499</li>
        </ul>
        
  <h4>devolverProductosAlInventario(Factura& factura)</h4>
        <p>Returns products from a canceled invoice to inventory:</p>
        <ul>
            <li>Matches products by name</li>
            <li>Increments existencia field for each returned product</li>
        </ul>
    </div>

  <div class="section">
        <h2>User Interface Components</h2>
        
  <h3>Main Menu</h3>
        <p>The entry point of the application with these options:</p>
        <ol>
            <li><strong>Emitir Factura</strong>: Create a new invoice</li>
            <li><strong>Buscar Factura</strong>: Search for an existing invoice</li>
            <li><strong>Mostrar Todas las Facturas</strong>: Display all invoices with sorting options</li>
            <li><strong>Mostrar Estado de Cuenta</strong>: Show sales summary</li>
            <li><strong>Mostrar Inventario</strong>: Display current product inventory</li>
            <li><strong>Agregar productos al inventario</strong>: Add new products</li>
            <li><strong>Salir</strong>: Exit the application</li>
        </ol>
        
  <h3>Invoice Creation Workflow</h3>
        <ol>
            <li>Enter customer name</li>
            <li>Add products by ID with quantities</li>
            <li>System checks stock availability</li>
            <li>Option to add more products</li>
            <li>Automatic NIT requirement check for totals > 2499</li>
            <li>Invoice editing menu with options to:
                <ul>
                    <li>Add more products</li>
                    <li>Remove products</li>
                    <li>Finalize invoice</li>
                    <li>Cancel invoice (returns products to inventory)</li>
                </ul>
            </li>
        </ol>
        
  <h3>Display Functions</h3>
        
  <h4>mostrarProductosDisponibles()</h4>
        <p>Displays available products in a formatted table with columns:</p>
        <ul>
            <li>ID (1-based index)</li>
            <li>Name</li>
            <li>Description</li>
            <li>Price</li>
            <li>Stock (existencia)</li>
        </ul>
        
  <h4>mostrarFactura(const Factura& factura)</h4>
        <p>Displays complete invoice information including:</p>
        <ul>
            <li>Business information</li>
            <li>Customer details</li>
            <li>Product list with quantities and totals</li>
            <li>Financial breakdown (subtotals, VAT, total)</li>
            <li>NIT/CF indication</li>
        </ul>
    </div>

<div class="section">
        <h1 align="center">Usage Guide</h1>
        
  <h3>Getting Started</h3>
        <ol>
            <li>Compile the program with a C++17 compatible compiler</li>
            <li>Ensure nlohmann/json.hpp is in your include path</li>
            <li>Run the executable</li>
            <li>The system will create a "datos.json" file if none exists</li>
        </ol>
        
  <h3>Adding Products</h3>
        <p>From the main menu, select option 6 and provide:</p>
        <ul>
            <li>Product name</li>
            <li>Description</li>
            <li>Price</li>
            <li>Initial stock quantity</li>
        </ul>
        
  <h3>Creating an Invoice</h3>
        <ol>
            <li>Select "Emitir Factura" from the main menu</li>
            <li>Enter customer name</li>
            <li>Add products by ID and quantity</li>
            <li>Continue adding products or finish</li>
            <li>Provide NIT if required or requested</li>
            <li>Use the invoice menu to finalize or make changes</li>
        </ol>
        
  <h3>Example Invoice Creation</h3>
        <pre>
1. Select "1. Emitir Factura"
2. Enter customer name: "Juan Perez"
3. View product list and note IDs:
   - 1 Laptop  Q5000 (10 in stock)
   - 2 Mouse   Q150 (25 in stock)
4. Add product 1, quantity 2
5. Add product 2, quantity 1
6. Choose not to add more products
7. System calculates total (Q10180 + 12% VAT)
8. Since total > Q2499, enter NIT: "12345678-9"
9. Finalize invoice
        </pre>
        
  <h3>Viewing Reports</h3>
        <p><strong>All Invoices:</strong> Option 3 with sorting by:
        <ul>
            <li>Invoice code</li>
            <li>Customer name</li>
            <li>Total amount</li>
        </ul>
        
  <p><strong>Sales Summary:</strong> Option 4 shows:
        <ul>
            <li>List of all invoices with totals</li>
            <li>Grand total of all sales</li>
        </ul>
    </div>

<div class="section">
        <h2>Important Notes</h2>
        
  <div class="note">
            <h4>Data Persistence</h4>
            <p>The system automatically saves data to "datos.json" when:</p>
            <ul>
                <li>Creating or canceling an invoice</li>
                <li>Adding new products</li>
                <li>Exiting the program</li>
            </ul>
        </div>
        
  <div class="warning">
            <h4>Tax Compliance</h4>
            <p>The system enforces NIT collection for invoices over Q2499 as required by Guatemalan tax law. 
            For smaller invoices, users can choose between NIT or CF (Consumidor Final).</p>
        </div>
        
   <h3>Technical Considerations</h3>
        <ul>
            <li>Product matching is done by name - ensure unique product names</li>
            <li>Invoice codes are sequential (FAC-1, FAC-2, etc.)</li>
            <li>The system uses Guatemalan VAT rate of 12%</li>
            <li>All monetary values are in Quetzales (Q)</li>
        </ul>
    </div>

  <div class="section">
        <h2>Conclusion</h2>  
  <p>
  Future enhancements could include barcode scanning support, user authentication, and more 
  detailed reporting features.
  </p>
  </div>

<h1 align="center"><b>Billing and Inventory Control System with PostgreSQL</b><img src="https://media.giphy.com/media/hvRJCLFzcasrR4ia7z/giphy.gif" width="35"></h1>
<br>


<div class="section">
    <h2>Overview</h2>
    <p>This billing and inventory control system is developed in modern C++ (C++17) and uses PostgreSQL as its database management system. Designed for "Tienda Facturación DB", it provides complete inventory management, invoice generation, and customer tracking.</p>
    
   <h3>Key Features</h3>
    <ul>
        <li>Robust PostgreSQL database connection</li>
        <li>Complete inventory management</li>
        <li>Invoice generation with automatic VAT calculation (12%)</li>
        <li>Differentiation between NIT-registered customers and final consumers</li>
        <li>Data validation and error handling</li>
        <li>Intuitive console interface</li>
    </ul>
</div>

<div class="section">
    <h2>Data Structures</h2>
    
   <h3>Product Structure</h3>
    <pre><code>struct Product {
    int id = 0;
    string name = "";
    string description = "";
    double price = 0.0;
    int stock = 0;
};</code></pre>
    <table>
        <tr>
            <th>Field</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Unique product identifier</td>
        </tr>
        <tr>
            <td>name</td>
            <td>string</td>
            <td>Product name</td>
        </tr>
        <tr>
            <td>description</td>
            <td>string</td>
            <td>Detailed product description</td>
        </tr>
        <tr>
            <td>price</td>
            <td>double</td>
            <td>Unit price</td>
        </tr>
        <tr>
            <td>stock</td>
            <td>int</td>
            <td>Available inventory quantity</td>
        </tr>
    </table>

   <h3>Customer Structure</h3>
    <pre><code>struct Customer {
    int id = 0;
    string firstName = "";
    string lastName = "";
    string address = "";
    string idNumber = "";
    string taxId = "";
};</code></pre>
    <table>
        <tr>
            <th>Field</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Unique customer identifier</td>
        </tr>
        <tr>
            <td>firstName</td>
            <td>string</td>
            <td>Customer's first name</td>
        </tr>
        <tr>
            <td>lastName</td>
            <td>string</td>
            <td>Customer's last name</td>
        </tr>
        <tr>
            <td>address</td>
            <td>string</td>
            <td>Customer's address</td>
        </tr>
        <tr>
            <td>idNumber</td>
            <td>string</td>
            <td>Personal identification number (13 digits)</td>
        </tr>
        <tr>
            <td>taxId</td>
            <td>string</td>
            <td>Tax identification number (NIT)</td>
        </tr>
    </table>

   <h3>Final Consumer Structure</h3>
    <pre><code>struct FinalConsumer {
    int id = 0;
    string name = "";
    string address = "";
};</code></pre>
    <table>
        <tr>
            <th>Field</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Unique final consumer identifier</td>
        </tr>
        <tr>
            <td>name</td>
            <td>string</td>
            <td>Consumer name</td>
        </tr>
        <tr>
            <td>address</td>
            <td>string</td>
            <td>Consumer address</td>
        </tr>
    </table>

   <h3>Invoice Structure</h3>
    <pre><code>struct Invoice {
    int id = 0;
    string code = "";
    string companyName = "";
    string companyAddress = "";
    string companyTaxId = "";
    int customer_id = 0;
    int finalConsumer_id = 0;
    double subtotalNoVat = 0.0;
    double subtotal = 0.0;
    double vat = 0.0;
    double total = 0.0;
    bool requiresTaxId = false;
    vector&lt;Product&gt; products;
};</code></pre>
    <table>
        <tr>
            <th>Field</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
        <tr>
            <td>id</td>
            <td>int</td>
            <td>Unique invoice identifier</td>
        </tr>
        <tr>
            <td>code</td>
            <td>string</td>
            <td>Automatically generated unique invoice code</td>
        </tr>
        <tr>
            <td>companyName</td>
            <td>string</td>
            <td>Issuing company name</td>
        </tr>
        <tr>
            <td>companyAddress</td>
            <td>string</td>
            <td>Issuing company address</td>
        </tr>
        <tr>
            <td>companyTaxId</td>
            <td>string</td>
            <td>Issuing company tax ID</td>
        </tr>
        <tr>
            <td>customer_id</td>
            <td>int</td>
            <td>Customer ID (if applicable)</td>
        </tr>
        <tr>
            <td>finalConsumer_id</td>
            <td>int</td>
            <td>Final consumer ID (if applicable)</td>
        </tr>
        <tr>
            <td>subtotalNoVat</td>
            <td>double</td>
            <td>Subtotal before VAT</td>
        </tr>
        <tr>
            <td>subtotal</td>
            <td>double</td>
            <td>Subtotal including VAT</td>
        </tr>
        <tr>
            <td>vat</td>
            <td>double</td>
            <td>Calculated VAT amount (12%)</td>
        </tr>
        <tr>
            <td>total</td>
            <td>double</td>
            <td>Total amount to pay</td>
        </tr>
        <tr>
            <td>requiresTaxId</td>
            <td>bool</td>
            <td>Indicates if invoice requires tax ID</td>
        </tr>
        <tr>
            <td>products</td>
            <td>vector&lt;Product&gt;</td>
            <td>List of products included in invoice</td>
        </tr>
    </table>
</div>

<div class="section">
    <h2>Core Functions</h2>
    
   <h3>Database Connection</h3>
    <h4>connectDB()</h4>
    <p>Establishes connection to PostgreSQL database:</p>
    <ul>
        <li>Uses configured connection parameters</li>
        <li>Handles connection errors</li>
        <li>Returns pqxx connection object</li>
    </ul>
    
   <h4>verifyTables(connection& C)</h4>
    <p>Verifies all required tables exist in database:</p>
    <ul>
        <li>products_inventory</li>
        <li>customers</li>
        <li>final_consumers</li>
        <li>invoices</li>
        <li>invoice_details</li>
    </ul>
    
   <h3>Inventory Management</h3>
    <h4>getInventory(connection& C)</h4>
    <p>Retrieves all inventory products ordered by ID:</p>
    <ul>
        <li>Executes SQL query</li>
        <li>Returns vector of Product</li>
    </ul>
    
   <h4>getProductById(connection& C, int id)</h4>
    <p>Gets specific product by ID:</p>
    <ul>
        <li>Throws exception if product not found</li>
        <li>Returns Product object</li>
    </ul>
    
   <h4>updateStock(connection& C, int product_id, int new_stock)</h4>
    <p>Updates product stock quantity:</p>
    <ul>
        <li>Returns true if update successful</li>
    </ul>
    
   <h4>reduceStock(connection& C, int product_id, int quantity)</h4>
    <p>Reduces product stock when sold:</p>
    <ul>
        <li>Verifies sufficient stock exists</li>
        <li>Returns true if operation successful</li>
    </ul>
    
   <h4>addProduct(connection& C, const Product& p)</h4>
    <p>Adds new product to inventory:</p>
    <ul>
        <li>Returns true if insertion successful</li>
    </ul>
    
   <h3>Billing</h3>
    <h4>saveInvoice(connection& C, Invoice& invoice, const string& customerName, const string& taxId, const string& address, const string& idNumber)</h4>
    <p>Saves complete invoice to database:</p>
    <ul>
        <li>Inserts customer (with tax ID or final consumer)</li>
        <li>Generates invoice record</li>
        <li>Saves invoice product details</li>
        <li>Returns true if operation successful</li>
    </ul>
    
   <h3>User Interface</h3>
    <h4>inventoryMenu(connection& C)</h4>
    <p>Displays inventory management menu with options to:</p>
    <ul>
        <li>View complete inventory</li>
        <li>Search product by ID</li>
        <li>Add new product</li>
        <li>Update stock quantities</li>
    </ul>
    
   <h4>createInvoice(connection& C)</h4>
    <p>Guides user through billing process:</p>
    <ul>
        <li>Captures customer data</li>
        <li>Allows product and quantity selection</li>
        <li>Automatically calculates totals</li>
        <li>Generates unique invoice code</li>
        <li>Displays summary before saving</li>
    </ul>
    
   <h4>mainMenu(connection& C)</h4>
    <p>Displays main menu with options to:</p>
    <ul>
        <li>Inventory management</li>
        <li>Invoice creation</li>
        <li>Exit system</li>
    </ul>
</div>

<div class="section">
    <h2>System Requirements</h2>
    
   <h3>Technical Requirements</h3>
    <ul>
        <li>C++17 compiler (g++ 7+, clang 5+, MSVC 2017+)</li>
        <li>PostgreSQL 9.5+</li>
        <li>libpqxx 6.0+ library</li>
    </ul>
    
   <h3>Database Configuration</h3>
    <p>Modify constants at code beginning according to your environment:</p>
    <pre><code>const string DB_NAME = "Tienda_Facturación_DB";
const string DB_USER = "ricardo";
const string DB_PASSWORD = "ricgio921";
const string DB_HOST = "localhost";
const string DB_PORT = "5432";</code></pre>
    
   <h3>Database Structure</h3>
    <p>System expects the following tables:</p>
    <ul>
        <li><strong>products_inventory</strong>: Stores product information</li>
        <li><strong>customers</strong>: Stores customers with tax ID</li>
        <li><strong>final_consumers</strong>: Stores final consumers</li>
        <li><strong>invoices</strong>: Invoice records</li>
        <li><strong>invoice_details</strong>: Product details per invoice</li>
    </ul>
</div>

<div class="section">
    <h2>Usage Guide</h2>
    
   <h3>Compilation and Execution</h3>
    <ol>
        <li>Compile with C++17 support:
            <pre>g++ -std=c++17 -o billing_system main.cpp -lpqxx -lpq</pre>
        </li>
        <li>Run the program:
            <pre>./billing_system</pre>
        </li>
    </ol>
    
   <h3>Workflow</h3>
    <h4>1. Inventory Management</h4>
    <ol>
        <li>Select "Inventory Management" from main menu</li>
        <li>Add products with name, description, price and initial stock</li>
        <li>Update stock according to sales or restocking</li>
    </ol>
    
   <h4>2. Invoice Creation</h4>
    <ol>
        <li>Select "Create Invoice" from main menu</li>
        <li>Enter customer data (name, address)</li>
        <li>Provide tax ID if required (invoices over Q2499)</li>
        <li>Select products and quantities</li>
        <li>Review summary and confirm</li>
    </ol>
    
   <h3>Billing Example</h3>
    <pre>
1. Select "2. Create Invoice"
2. Enter name: "Maria Lopez"
3. Enter address: "5th avenue 8-90 zone 1"
4. Enter tax ID: "98765432-1" (required for invoices > Q2499)
5. Enter ID number: "1234567890123" (13 digits)
6. Select products:
   - ID 1: Laptop (Q5000) x 2 units
   - ID 3: Mouse (Q150) x 1 unit
7. Confirm invoice
8. System shows summary with:
   - Subtotal: Q10150
   - VAT (12%): Q1089.29
   - Total: Q10150
9. Invoice saved with unique code (e.g.: FAC-1623456789-456)</pre>
</div>

<div class="section">
    <h2>Important Considerations</h2>
    
   <div class="warning">
        <h4>Tax Compliance</h4>
        <p>The system validates that invoices over Q2499 include customer tax ID, complying with Guatemalan tax regulations.</p>
    </div>
    
   <div class="note">
        <h4>Data Security</h4>
        <p>All information is stored in PostgreSQL, providing:</p>
        <ul>
            <li>Data integrity</li>
            <li>ACID transactions</li>
            <li>Backup and recovery</li>
        </ul>
    </div>
    
   <h3>Recommendations</h3>
    <ul>
        <li>Perform regular database backups</li>
        <li>Configure users with appropriate permissions</li>
        <li>Implement private network for database connection in production environments</li>
    </ul>
</div>

<div class="section">
    <h2>Potential Improvements</h2>
    <ul>
        <li>Implement user authentication</li>
        <li>Add detailed sales reports</li>
        <li>Implement advanced invoice search</li>
        <li>Develop graphical interface</li>
        <li>Add multi-branch support</li>
    </ul>
</div>

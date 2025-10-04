# React + Vite + Tailwindcss

## Comandos Rápidos de Inicio

```bash
# 1. Actualizar npm a la última versión
npm install -g npm@latest

# 3. Instalar dependencias
npm install

# 4. Ejecutar el servidor de desarrollo
npm run dev
```

## Dependencias del Proyecto
### **react-router-dom**
### **@tanstack/react-query**
### **axios**
### **react-hook-form**
### **@hookform/resolvers**
### **yup**
### **lucide-react**
### **MySweetAlert2

---

## TOKENS DE DISEÑO

### **Colores Principales**
```css
- Primario: slate-900 (#0f172a) - Negro profundo
- Primario Hover: slate-800 (#1e293b) 
- Acento: blue-700 (#1d4ed8) - Azul corporativo
- Acento Light: blue-100 (#dbeafe)
- Superficie: white (#ffffff)
- Fondo: slate-50 (#f8fafc)
- Fondo Alternativo: slate-100 (#f1f5f9)
- Texto Principal: slate-900 (#0f172a)
- Texto Secundario: slate-600 (#475569)
- Texto Terciario: slate-500 (#64748b)
```

### **Bordes y Estados**
```css
- Bordes Suaves: border-slate-200 (#e2e8f0)
- Bordes Hover: border-slate-500 (#64748b)
- Bordes Definidos: border-slate-300 (#cbd5e1)
- Hover States: hover:bg-slate-50, hover:bg-slate-100
- Gradiente Hero: from-slate-50 to-slate-100
```

### **Elementos Específicos**
```css
- Logo Container: bg-slate-900
- Texto Destacado Hero: text-blue-700
- Indicador Activo: bg-blue-600
- Botón Primario: bg-slate-900 hover:bg-slate-800
- Botones Secundarios: bg-white border-slate-300 hover:border-slate-500
- CTA Final: bg-slate-900 con text-slate-300 para subtextos
```

### **Iconos y Estadísticas**
```css
- Tiendas: bg-blue-100 text-blue-700
- Pedidos: bg-slate-200 text-slate-700  
- Tiempo: bg-indigo-100 text-indigo-700
- Pasos Proceso: bg-slate-100 border-slate-200 text-slate-700
- Números Pasos: bg-slate-900
```

### **Sombras y Elevación**
```css
- Header: shadow-lg
- Cards: shadow-sm hover:shadow-lg
- Botones Principales: shadow-xl hover:shadow-2xl
- Estadísticas: shadow-sm con borders
- Información Costos: shadow-lg
```

### **Tipografía** *(Mantenida consistente)*
```css
- Hero: text-5xl font-bold text-slate-900
- Títulos H2: text-3xl font-bold text-slate-900
- Títulos H3: text-xl font-semibold text-slate-900
- Botones principales: text-xl font-semibold
- Botones secundarios: text-lg font-semibold
- Cuerpo: text-slate-600
- Pequeño: text-slate-500
```

### **Espaciado y Layout** *(Sin cambios)*
```css
- Padding contenedores: px-4 sm:px-6 lg:px-8
- Secciones: py-16
- Botones principales: px-10 py-5 rounded-xl
- Botones secundarios: px-8 py-4 rounded-xl
- Transiciones: transition-all duration-200
- Hover effects: transform hover:scale-105
```

**Características del diseño profesional:**
- **Contraste alto**: Slate-900 sobre fondos claros
- **Jerarquía visual clara**: Diferencias marcadas entre elementos
- **Bordes sutiles**: Para definir secciones sin ser intrusivos  
- **Acentos azules**: Solo para elementos importantes (CTA, enlaces)
- **Sombras estratégicas**: Para crear profundidad y elegancia
- **Paleta monocromática**: Con toques de color solo donde es necesario

## **6 PRINCIPIOS DE JAKOB NIELSEN APLICADOS**

### **Visibilidad del Estado del Sistema**
```typescript
- Hover states con cambios de color y escala
- Estados loading con spinners
- Botones deshabilitados durante procesos
- Indicadores visuales claros (iconos + texto)
```

### **Prevención de Errores**
```typescript
- Validación en tiempo real con React Hook Form
- Tipos de input apropiados (email, tel, password)
- Botones deshabilitados cuando hay errores
- Confirmaciones antes de acciones destructivas
```

### **Ayuda a Reconocer, Diagnosticar y Recuperarse de Errores**
```typescript
- Mensajes de error específicos por campo
- Iconos de alerta consistentes (AlertCircle de Lucide)
- Colores diferenciados (rojo para errores)
- Sugerencias constructivas de corrección
```

### **Reconocimiento antes que Memorización**
```typescript
- Labels descriptivos y placeholders útiles
- Iconos intuitivos (Lucide React)
- Indicadores visuales de campos requeridos (*)
- Ejemplos de formato cuando sea necesario
```

### **Consistencia y Estándares**
```typescript
- Esquema de colores uniforme (emerald + slate)
- Iconografía consistente (solo Lucide React)
- Patrones de botones estandarizados
- Nomenclatura coherente en toda la app
- Espaciado sistemático con Tailwind
```

### **Coincidencia entre el sistema y el mundo real**
```typescript
- Iconos familiares: Truck (entrega), Store (tienda), Users (clientes)
- Terminología común: "Carrito", "Pedido", "Entrega"
- Flujo natural: Explorar → Agregar → Pagar → Recibir
- Metáforas conocidas: "Puerta de tu casa"
```
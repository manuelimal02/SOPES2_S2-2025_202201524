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

# Tokens de Diseño - Escritorio Remoto
## Paleta Profesional Verde Bosque + Grises Cálidos

### **Colores Principales**
```css
/* VERDES PROFESIONALES */
- Primario: emerald-900 (#064e3b) - Verde bosque profundo
- Primario Hover: emerald-800 (#065f46) 
- Acento: emerald-600 (#059669) - Verde corporativo brillante
- Acento Light: emerald-50 (#ecfdf5) - Fondo suave verde
- Acento Medio: emerald-100 (#d1fae5) - Destacados sutiles

/* GRISES CÁLIDOS (ZINC) */
- Superficie: white (#ffffff)
- Fondo: zinc-50 (#fafafa) - Gris muy claro cálido
- Fondo Alternativo: zinc-100 (#f4f4f5) - Gris claro
- Fondo Sidebar: zinc-900 (#18181b) - Gris oscuro cálido
- Texto Principal: zinc-900 (#18181b)
- Texto Secundario: zinc-600 (#52525b)
- Texto Terciario: zinc-500 (#71717a)
- Texto Sobre Oscuro: zinc-100 (#f4f4f5)
```

### **Bordes y Estados**
```css
/* BORDES SUTILES */
- Bordes Base: border-zinc-200 (#e4e4e7)
- Bordes Hover: border-emerald-500 (#10b981)
- Bordes Definidos: border-zinc-300 (#d4d4d8)
- Bordes Activos: border-emerald-600 (#059669)

/* HOVER STATES */
- Hover Superficie: hover:bg-zinc-50
- Hover Cards: hover:bg-zinc-100
- Hover Primario: hover:bg-emerald-800
- Hover Secundario: hover:border-emerald-500

/* GRADIENTES */
- Hero Gradient: from-zinc-50 to-zinc-100
- Card Gradient: from-emerald-50 to-white
```

### **Elementos Específicos**
```css
/* NAVEGACIÓN Y ESTRUCTURA */
- Header/Sidebar: bg-zinc-900
- Logo Container: bg-emerald-900
- Indicador Activo: bg-emerald-600
- Menu Item Hover: bg-zinc-800
- Menu Item Active: bg-emerald-900/20

/* BOTONES */
- Botón Primario: bg-emerald-900 hover:bg-emerald-800
- Botón Primario Text: text-white
- Botón Secundario: bg-white border-zinc-300 hover:border-emerald-500
- Botón Secundario Text: text-zinc-900
- Botón Terciario: bg-emerald-50 text-emerald-900 hover:bg-emerald-100
- Botón Peligro: bg-red-600 hover:bg-red-700

/* CTA Y LLAMADOS A LA ACCIÓN */
- CTA Principal: bg-emerald-900 text-white
- CTA Subtexto: text-emerald-100
- CTA Hover: hover:bg-emerald-800 hover:shadow-emerald-900/50
```

### **Iconos y Estados Visuales**
```css
/* ICONOS INFORMATIVOS */
- Conexión Activa: bg-emerald-100 text-emerald-700
- Escritorios: bg-zinc-200 text-zinc-700  
- Sesiones: bg-emerald-50 text-emerald-800
- Usuarios: bg-zinc-100 text-zinc-600
- Alertas: bg-amber-100 text-amber-700
- Errores: bg-red-100 text-red-700

/* INDICADORES DE ESTADO */
- Online: bg-emerald-500 (dot indicator)
- Ocupado: bg-amber-500
- Offline: bg-zinc-400
- Error: bg-red-500

/* PASOS Y PROCESOS */
- Paso Container: bg-zinc-100 border-zinc-200
- Paso Text: text-zinc-700
- Número Paso: bg-emerald-900 text-white
- Paso Completado: bg-emerald-600
```

### **Sombras y Elevación**
```css
/* SOMBRAS PROFESIONALES */
- Header: shadow-lg shadow-zinc-900/10
- Cards: shadow-sm hover:shadow-lg hover:shadow-emerald-900/10
- Botones Principales: shadow-xl shadow-emerald-900/30 hover:shadow-2xl
- Modal/Dropdown: shadow-2xl shadow-zinc-900/20
- Estadísticas: shadow-sm border border-zinc-200
- Información Destacada: shadow-lg shadow-emerald-900/10
```

### **Tipografía** *(Optimizada para profesionalismo)*
```css
/* TÍTULOS */
- Hero: text-5xl font-bold text-zinc-900
- H1: text-4xl font-bold text-zinc-900
- H2: text-3xl font-bold text-zinc-900
- H3: text-xl font-semibold text-zinc-900
- H4: text-lg font-semibold text-zinc-800

/* DESTACADOS */
- Texto Destacado Hero: text-emerald-700
- Números/Estadísticas: text-emerald-600 font-bold
- Labels: text-zinc-700 font-medium

/* BOTONES */
- Botón Principal: text-xl font-semibold
- Botón Secundario: text-lg font-semibold
- Botón Pequeño: text-sm font-medium

/* CUERPO */
- Párrafo Principal: text-zinc-600
- Párrafo Secundario: text-zinc-500
- Caption/Pequeño: text-sm text-zinc-500
- Placeholder: text-zinc-400
```

### **Espaciado y Layout** *(Consistente)*
```css
/* CONTENEDORES */
- Padding General: px-4 sm:px-6 lg:px-8
- Secciones: py-16
- Cards: p-6 rounded-xl
- Sidebar: w-64 (desktop) w-full (mobile)

/* BOTONES */
- Botón Grande: px-10 py-5 rounded-xl
- Botón Medio: px-8 py-4 rounded-xl
- Botón Pequeño: px-6 py-3 rounded-lg
- Botón Mini: px-4 py-2 rounded-md

/* ANIMACIONES */
- Transiciones: transition-all duration-200
- Hover Transform: transform hover:scale-105
- Smooth Scroll: scroll-smooth
```

### **Inputs y Formularios**
```css
/* CAMPOS DE ENTRADA */
- Input Base: bg-white border-zinc-300 text-zinc-900
- Input Focus: focus:border-emerald-600 focus:ring-emerald-600
- Input Error: border-red-500 focus:border-red-500
- Input Disabled: bg-zinc-100 text-zinc-400

/* LABELS Y HINTS */
- Label: text-zinc-700 font-medium
- Hint Text: text-zinc-500 text-sm
- Error Message: text-red-600 text-sm
- Success Message: text-emerald-600 text-sm
```

---

## **CARACTERÍSTICAS DEL DISEÑO PROFESIONAL**

### **Paleta Verde Bosque + Zinc**
✅ **Profesionalismo**: Verde oscuro (#064e3b) transmite estabilidad y confianza  
✅ **Calidez**: Zinc es más cálido que slate, perfecto para interfaces de uso prolongado  
✅ **Contraste Alto**: Zinc-900 sobre fondos claros garantiza legibilidad  
✅ **Acentos Estratégicos**: Verde esmeralda brillante solo en CTAs y elementos clave  

### **Jerarquía Visual Clara**
✅ Diferencias marcadas entre primario (emerald-900) y acento (emerald-600)  
✅ Grises zinc crean profundidad sin frialdad excesiva  
✅ Bordes sutiles definen sin ser intrusivos  

### **Estados y Feedback**
✅ Verde para estados positivos (conectado, éxito, activo)  
✅ Ámbar para advertencias (ocupado, atención)  
✅ Rojo solo para errores críticos  
✅ Zinc para estados neutrales (offline, inactivo)  

### **Sombras Estratégicas**
✅ Sombras con tinte del color principal (emerald-900/10)  
✅ Elevación progresiva: sm → lg → xl → 2xl  
✅ Sombras más pronunciadas en elementos interactivos  

---

## **6 PRINCIPIOS DE JAKOB NIELSEN APLICADOS**

### **1. Visibilidad del Estado del Sistema**
```typescript
✅ Indicadores de conexión con colores (emerald = conectado)
✅ Hover states con cambios de color y escala
✅ Estados loading con spinners en emerald-600
✅ Botones deshabilitados con zinc-400
✅ Breadcrumbs para navegación contextual
```

### **2. Prevención de Errores**
```typescript
✅ Validación en tiempo real con React Hook Form
✅ Tipos de input apropiados (text, password)
✅ Botones deshabilitados durante procesos
✅ Confirmaciones antes de cerrar sesiones remotas
✅ Tooltips explicativos en acciones críticas
```

### **3. Ayuda a Reconocer, Diagnosticar y Recuperarse de Errores**
```typescript
✅ Mensajes de error específicos por campo
✅ Iconos de alerta consistentes (AlertCircle de Lucide)
✅ Colores diferenciados (red-600 para errores)
✅ Sugerencias constructivas: "Verifica tu conexión a internet"
✅ Toast notifications para feedback inmediato
```

### **4. Reconocimiento antes que Memorización**
```typescript
✅ Labels descriptivos y placeholders útiles
✅ Iconos intuitivos (Monitor, Server, Users de Lucide)
✅ Indicadores visuales de campos requeridos (*)
✅ Breadcrumbs muestran ubicación actual
✅ Atajos de teclado visibles en tooltips
```

### **5. Consistencia y Estándares**
```typescript
✅ Esquema de colores uniforme (emerald-900 + zinc)
✅ Iconografía consistente (solo Lucide React)
✅ Patrones de botones estandarizados
✅ Nomenclatura coherente: "Escritorio", "Sesión", "Conexión"
✅ Espaciado sistemático con Tailwind
✅ Mismo estilo de cards en toda la app
```

### **6. Coincidencia entre el sistema y el mundo real**
```typescript
✅ Iconos familiares: Monitor (escritorio), Server (servidor), Wifi (conexión)
✅ Terminología técnica pero clara: "Escritorio Remoto", "Sesión Activa"
✅ Flujo natural: Seleccionar → Conectar → Trabajar → Desconectar
✅ Metáforas conocidas: "Escritorio" en lugar de "VM"
✅ Estados con lenguaje humano: "Conectando..." en lugar de "Init..."
```

---

## **COMPARATIVA DE CAMBIOS**

| Elemento | Antes (Azul/Slate) | Ahora (Verde/Zinc) |
|----------|-------------------|-------------------|
| **Primario** | slate-900 | emerald-900 |
| **Acento** | blue-700 | emerald-600 |
| **Grises** | slate (frío) | zinc (cálido) |
| **Hover CTA** | slate-800 | emerald-800 |
| **Iconos Estado** | blue-100 | emerald-100 |
| **Sombras** | genéricas | emerald-900/10 |

---

## **IMPLEMENTACIÓN EN CÓDIGO**

### Ejemplo de Botón Primario:
```jsx
<button className="bg-emerald-900 hover:bg-emerald-800 text-white px-10 py-5 rounded-xl shadow-xl shadow-emerald-900/30 hover:shadow-2xl transition-all duration-200 transform hover:scale-105">
  Conectar Escritorio
</button>
```

### Ejemplo de Card:
```jsx
<div className="bg-white p-6 rounded-xl shadow-sm hover:shadow-lg hover:shadow-emerald-900/10 border border-zinc-200 transition-all duration-200">
  <div className="flex items-center gap-3">
    <div className="p-3 bg-emerald-100 rounded-lg">
      <Monitor className="w-6 h-6 text-emerald-700" />
    </div>
    <div>
      <h3 className="text-xl font-semibold text-zinc-900">Escritorio Principal</h3>
      <p className="text-zinc-600">Última conexión hace 2 horas</p>
    </div>
  </div>
</div>
```

### Ejemplo de Indicador de Estado:
```jsx
<div className="flex items-center gap-2">
  <span className="w-2 h-2 rounded-full bg-emerald-500"></span>
  <span className="text-sm text-zinc-600">Conectado</span>
</div>
```
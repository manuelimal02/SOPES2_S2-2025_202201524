import { Routes, Route, Navigate } from "react-router-dom";
import Login from "./vistas/Login";
import Dashboard from "./vistas/Dashboard";

function RutaProtegida({ children }) {
  const usuario = localStorage.getItem("usuario");
  return usuario ? children : <Navigate to="/" replace />;
}

function App() {
  return (
    <Routes>
      {/* Ruta pública */}
      <Route path="/" element={<Login />} />

      {/* Ruta protegida */}
      <Route
        path="/dashboard"
        element={
          <RutaProtegida>
            <Dashboard />
          </RutaProtegida>
        }
      />

      {/* Redirección en caso de ruta inexistente */}
      <Route path="*" element={<Navigate to="/" />} />
    </Routes>
  );
}

export default App;

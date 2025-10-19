import { useState, useEffect } from 'react';
import { Cpu, HardDrive, LogOut, User, Eye, Shield, Activity, Mouse, Keyboard, Monitor } from 'lucide-react';
import Swal from 'sweetalert2';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';

export default function DashboardRecursos() {
    const [usoCpu, setUsoCpu] = useState(0);
    const [cpuNoUsado, setCpuNoUsado] = useState(0);
    const [usoRam, setUsoRam] = useState(0);
    const [ramNoUsada, setRamNoUsada] = useState(0);
    const [nombreUsuario, setNombreUsuario] = useState('Usuario');
    const [grupoUsuario, setGrupoUsuario] = useState('view_only');
    const [error, setError] = useState(null);
    const [imagenPantalla, setImagenPantalla] = useState(null);
    const [ultimaActualizacion, setUltimaActualizacion] = useState(null);
    const [ultimoInput, setUltimoInput] = useState(null);
    const [resolucion, setResolucion] = useState({ ancho: 1920, alto: 1080 });
    const navigate = useNavigate();

    useEffect(() => {
        const usuarioGuardado = localStorage.getItem('usuario');
        const grupoGuardado = localStorage.getItem('grupo');
        if (usuarioGuardado) {
            try {
                const datosUsuario = JSON.parse(usuarioGuardado);
                setNombreUsuario(datosUsuario);
            } catch {
                setNombreUsuario('Usuario Demo');
            }
        }
        if (grupoGuardado) {
            try {
                const grupo = JSON.parse(grupoGuardado);
                setGrupoUsuario(grupo);
            } catch {
                setGrupoUsuario('view_only');
            }
        }
    }, []);

    useEffect(() => {
        const obtenerDatosCpu = async () => {
            try {
                const { data } = await axios.get('http://localhost:8081/cpu');
                if (data.error) {
                    setError(data.error);
                } else {
                    setUsoCpu(data.porcentaje_usado);
                    setCpuNoUsado(data.porcentaje_no_usado);
                    setError(null);
                }
            } catch (err) {
                console.error('Error al obtener CPU:', err);
                setError('No se pudo conectar con la API');
            }
        };
        obtenerDatosCpu();
        const intervalo = setInterval(obtenerDatosCpu, 2000);
        return () => clearInterval(intervalo);
    }, []);

    useEffect(() => {
        const obtenerDatosRam = async () => {
            try { 
                const { data } = await axios.get('http://localhost:8081/ram');
                if (data.error) {
                    setError(data.error);
                } else {
                    setUsoRam(data.porcentaje_usado);
                    setRamNoUsada(data.porcentaje_no_usado);
                    setError(null);
                }
            } catch (err) {
                console.error('Error al obtener RAM:', err);
                setError('No se pudo conectar con la API');
            }
        };
        obtenerDatosRam();
        const intervalo = setInterval(obtenerDatosRam, 2000);
        return () => clearInterval(intervalo);
    }, []);

    const manejarCerrarSesion = () => {
            Swal.fire({
                title: '¿Cerrar sesión?',
                text: 'Tu sesión actual se cerrará.',
                icon: 'warning',
                showCancelButton: true,
                confirmButtonColor: '#059669',
                cancelButtonColor: '#dc2626',
                confirmButtonText: 'Cerrar Sesión',
                cancelButtonText: 'Cancelar',
            }).then((result) => {
                if (result.isConfirmed) {
                    localStorage.removeItem('usuario');
                    localStorage.removeItem('grupo');
                    Swal.fire({
                        icon: 'success',
                        title: 'Sesión Cerrada',
                        text: 'Has cerrado sesión correctamente.',
                        confirmButtonColor: '#059669',
                        timer: 1500,
                        showConfirmButton: false,
                    });
                    navigate('/');
                }
            });
        };
    


    return (
        <div className="min-h-screen bg-zinc-50">
            <header className="bg-white border-b border-zinc-200 shadow-sm">
                <div className="max-w-7xl mx-auto px-6 py-4">
                    <div className="flex justify-between items-center gap-8">
                        <div className="flex items-center gap-3 min-w-[200px]">
                            <div className="bg-emerald-900 p-2.5 rounded-lg shadow-lg shadow-emerald-900/30">
                                <Shield className="w-6 h-6 text-white" />
                            </div>
                            <div>
                                <h1 className="text-xl font-bold text-zinc-900">USACLinux</h1>
                                <p className="text-xs text-zinc-500">Escritorio Remoto</p>
                            </div>
                        </div>

                        <div className="flex items-center gap-4 flex-1 justify-center">
                            <div className="flex items-center gap-3 bg-emerald-50 px-5 py-3 rounded-xl border border-emerald-200 shadow-sm hover:shadow-md transition-shadow">
                                <div className="bg-emerald-700 p-2.5 rounded-lg shadow-md">
                                    <Cpu className="w-5 h-5 text-white" />
                                </div>
                                <div>
                                    <div className="text-xs font-medium text-zinc-600 mb-1.5">CPU</div>
                                    <div className="flex items-baseline gap-3">
                                        <div className="text-center">
                                            <div className="text-xl font-bold text-zinc-900">{usoCpu}%</div>
                                            <div className="text-xs text-zinc-500">Usado</div>
                                        </div>
                                        <div className="w-px h-10 bg-zinc-300"></div>
                                        <div className="text-center">
                                            <div className="text-xl font-bold text-emerald-600">{cpuNoUsado}%</div>
                                            <div className="text-xs text-zinc-500">Libre</div>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            <div className="flex items-center gap-3 bg-emerald-50 px-5 py-3 rounded-xl border border-emerald-200 shadow-sm hover:shadow-md transition-shadow">
                                <div className="bg-emerald-700 p-2.5 rounded-lg shadow-md">
                                    <HardDrive className="w-5 h-5 text-white" />
                                </div>
                                <div>
                                    <div className="text-xs font-medium text-zinc-600 mb-1.5">RAM</div>
                                    <div className="flex items-baseline gap-3">
                                        <div className="text-center">
                                            <div className="text-xl font-bold text-zinc-900">{usoRam}%</div>
                                            <div className="text-xs text-zinc-500">Usado</div>
                                        </div>
                                        <div className="w-px h-10 bg-zinc-300"></div>
                                        <div className="text-center">
                                            <div className="text-xl font-bold text-emerald-600">{ramNoUsada}%</div>
                                            <div className="text-xs text-zinc-500">Libre</div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>

                        <div className="flex items-center gap-3 min-w-[300px] justify-end">
                            {ultimaActualizacion && (
                                <div className="flex items-center gap-2 bg-zinc-100 px-3 py-2 rounded-lg border border-zinc-200">
                                    <Activity className="w-4 h-4 text-emerald-600" />
                                    <div>
                                        <div className="text-xs text-zinc-500">Actualizado</div>
                                        <div className="text-sm font-semibold text-zinc-900">{ultimaActualizacion}</div>
                                    </div>
                                </div>
                            )}

                            <div className="flex items-center gap-2 bg-zinc-100 px-4 py-2 rounded-lg border border-zinc-200">
                                <User className="w-4 h-4 text-zinc-600" />
                                <span className="font-semibold text-zinc-900">{nombreUsuario}</span>
                            </div>

                            <button 
                                onClick={manejarCerrarSesion}
                                className="flex items-center gap-2 bg-emerald-900 hover:bg-emerald-800 text-white px-4 py-2 rounded-lg transition-all shadow-md hover:shadow-lg hover:shadow-emerald-900/30"
                            >
                                <LogOut className="w-4 h-4" />
                                <span className="font-semibold">Salir</span>
                            </button>
                        </div>
                    </div>

                    {error && (
                        <div className="mt-4 bg-red-50 border border-red-200 rounded-lg p-3">
                            <p className="text-sm text-red-700">⚠️ {error}</p>
                        </div>
                    )}
                </div>
            </header>

        </div>
    );
}
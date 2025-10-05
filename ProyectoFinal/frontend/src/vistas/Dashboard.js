import { useState, useEffect } from 'react';
import { Cpu, HardDrive, LogOut, User, Eye, Shield, Activity, Mouse, Keyboard, Monitor } from 'lucide-react';
import Swal from 'sweetalert2';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';

export default function Dashboard() {
    const [usoCpu, setUsoCpu] = useState(0);
    const [cpuNoUsado, setCpuNoUsado] = useState(0);
    const [usoRam, setUsoRam] = useState(0);
    const [ramNoUsada, setRamNoUsada] = useState(0);
    const [nombreUsuario, setNombreUsuario] = useState('Usuario');
    const [grupoUsuario, setGrupoUsuario] = useState('view_only');
    const [error, setError] = useState(null);
    const [imagenPantalla, setImagenPantalla] = useState(null);
    const [ultimaActualizacion, setUltimaActualizacion] = useState(null);
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

    async function ppmToPng(blob) {
        const buffer = await blob.arrayBuffer();
        const data = new Uint8Array(buffer);
        const text = new TextDecoder().decode(data.slice(0, 64));
        const headerMatch = text.match(/P6\s+(\d+)\s+(\d+)\s+255\s/);

        if (!headerMatch) {
            throw new Error("Formato PPM inválido");
        }

        const width = parseInt(headerMatch[1]);
        const height = parseInt(headerMatch[2]);
        const headerEnd = text.indexOf('255') + 4;
        const pixelData = data.slice(headerEnd);

        const canvas = document.createElement('canvas');
        canvas.width = width;
        canvas.height = height;
        const ctx = canvas.getContext('2d');
        const imgData = ctx.createImageData(width, height);

        for (let i = 0, j = 0; i < pixelData.length; i += 3, j += 4) {
            imgData.data[j] = pixelData[i];
            imgData.data[j + 1] = pixelData[i+1];
            imgData.data[j + 2] = pixelData[i+2];
            imgData.data[j + 3] = 255;
        }

        ctx.putImageData(imgData, 0, 0);
        const pngBlob = await new Promise(res => canvas.toBlob(res, 'image/png'));
        return URL.createObjectURL(pngBlob);
    }

    useEffect(() => {
        const obtenerResolucion = async () => {
            try {
                const { data } = await axios.get('http://localhost:8081/resolucion');
                if (!data.error) {
                    setResolucion({ ancho: data.ancho, alto: data.alto });
                }
            } catch (err) {
                console.error('Error al obtener resolución:', err);
            }
        };
        obtenerResolucion();
        const intervalo = setInterval(obtenerResolucion, 20000);
        return () => clearInterval(intervalo);
    }, []);

    useEffect(() => {
        const obtenerPantalla = async () => {
            try {
                const respuesta = await axios.get('http://localhost:8081/pantalla', {
                    responseType: 'blob'
                });
                const urlImagen = await ppmToPng(respuesta.data);
                setImagenPantalla(urlImagen);
                setUltimaActualizacion(new Date().toLocaleTimeString());
            } catch (err) {
                console.error('Error al obtener pantalla:', err);
            }
        };
        obtenerPantalla();
        const intervalo = setInterval(obtenerPantalla, 1000);
        return () => clearInterval(intervalo);
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

    const enviarClickMouse = async (x, y, button) => {
        try {
            const { data } = await axios.post('http://localhost:8081/control-mouse', {
                x: Math.round(x),
                y: Math.round(y),
                button: button
            }, {
                headers: {
                    'Content-Type': 'application/json'
                }
            });
            
            if (data.error) {
                console.error('Error en control-mouse:', data.error);
                Swal.fire({
                    icon: 'error',
                    title: 'Error',
                    text: data.error,
                    toast: true,
                    position: 'top-end',
                    showConfirmButton: false,
                    timer: 2000
                });
            } else {
                console.log('Click enviado exitosamente:', data);
            }
        } catch (err) {
            console.error('Error al enviar click:', err);
            Swal.fire({
                icon: 'error',
                title: 'Error de Conexión',
                text: 'No se pudo enviar el click al servidor',
                toast: true,
                position: 'top-end',
                showConfirmButton: false,
                timer: 2000
            });
        }
    };

    const manejarMouseDown = (e) => {
        if (!tieneControlTotal) return;
        
        e.preventDefault();
        
        const rect = e.currentTarget.getBoundingClientRect();
        const xPorcentaje = ((e.clientX - rect.left) / rect.width) * 100;
        const yPorcentaje = ((e.clientY - rect.top) / rect.height) * 100;
        const xPixel = (xPorcentaje / 100) * resolucion.ancho;
        const yPixel = (yPorcentaje / 100) * resolucion.alto;
        const button = e.button === 2 ? 1 : 0;
        
        console.log(`Click: ${xPixel.toFixed(0)}x, ${yPixel.toFixed(0)}y, botón: ${button === 0 ? 'izquierdo' : 'derecho'}`);
        
        enviarClickMouse(xPixel, yPixel, button);
    };

    const manejarContextMenu = (e) => {
        if (tieneControlTotal) {
            e.preventDefault();
        }
    };

    const tieneControlTotal = grupoUsuario === 'remote_control';

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

            <main className="max-w-7xl mx-auto px-6 py-6">
                <div className="bg-white rounded-xl shadow-lg border border-zinc-200 overflow-hidden">
                    <div className="bg-emerald-900 px-6 py-4 flex justify-between items-center">
                        <h2 className="text-lg font-bold text-white">Escritorio Remoto</h2>
                        <div className="flex items-center gap-2">
                            <div className="w-2 h-2 bg-emerald-400 rounded-full animate-pulse"></div>
                            <span className="text-sm text-emerald-100">Activo</span>
                        </div>
                    </div>

                    <div className="bg-zinc-900 p-6 flex justify-center items-center">
                        <div 
                            className={`bg-zinc-800 rounded-lg overflow-hidden border-2 border-zinc-700 relative ${tieneControlTotal ? 'cursor-crosshair' : 'cursor-not-allowed'}`}
                            style={{ 
                                maxWidth: '100%',
                                maxHeight: '70vh',
                                aspectRatio: `${resolucion.ancho} / ${resolucion.alto}`
                            }}
                            onMouseDown={manejarMouseDown}
                            onContextMenu={manejarContextMenu}
                        >
                            {imagenPantalla ? (
                                <img 
                                    src={imagenPantalla} 
                                    alt="Escritorio remoto"
                                    className="w-full h-full object-cover"
                                    draggable={false}
                                />
                            ) : (
                                <div className="w-full h-full flex items-center justify-center">
                                    <div className="text-center">
                                        <Activity className="w-12 h-12 text-zinc-600 mx-auto mb-2 animate-pulse" />
                                        <p className="text-zinc-400">Cargando escritorio...</p>
                                    </div>
                                </div>
                            )}
                            
                            {!tieneControlTotal && (
                                <div className="absolute inset-0 bg-zinc-900 bg-opacity-30 flex items-center justify-center pointer-events-none">
                                    <div className="bg-emerald-600 text-white px-4 py-2 rounded-lg flex items-center gap-2 shadow-lg">
                                        <Eye className="w-5 h-5" />
                                        <span className="font-semibold">Solo Vista</span>
                                    </div>
                                </div>
                            )}
                        </div>
                    </div>

                    <div className={`px-6 py-4 ${tieneControlTotal ? 'bg-emerald-50 border-t border-emerald-200' : 'bg-zinc-100 border-t border-zinc-200'}`}>
                        <div className="flex items-center justify-between">
                            <div className="flex items-center gap-3">
                                {tieneControlTotal ? (
                                    <>
                                        <div className="bg-emerald-600 p-2.5 rounded-lg shadow-md">
                                            <Shield className="w-5 h-5 text-white" />
                                        </div>
                                        <div>
                                            <h3 className="font-semibold text-zinc-900">Acceso Total</h3>
                                            <p className="text-sm text-zinc-600">Puedes ver y controlar el escritorio (clic izquierdo y derecho)</p>
                                        </div>
                                    </>
                                ) : (
                                    <>
                                        <div className="bg-zinc-600 p-2.5 rounded-lg shadow-md">
                                            <Eye className="w-5 h-5 text-white" />
                                        </div>
                                        <div>
                                            <h3 className="font-semibold text-zinc-900">Solo Vista</h3>
                                            <p className="text-sm text-zinc-600">Puedes ver el escritorio sin controlarlo</p>
                                        </div>
                                    </>
                                )}
                            </div>

                            <div className="flex gap-2">
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-zinc-100 border-zinc-300 text-zinc-400'}`}>
                                    <Mouse className="w-4 h-4" />
                                    <span className="text-sm font-semibold">Mouse</span>
                                </div>
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-zinc-100 border-zinc-300 text-zinc-400'}`}>
                                    <Keyboard className="w-4 h-4" />
                                    <span className="text-sm font-semibold">Teclado</span>
                                </div>
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-zinc-100 border-zinc-300 text-zinc-400'}`}>
                                    <Monitor className="w-4 h-4" />
                                    <span className="text-sm font-semibold">Pantalla</span>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </main>
        </div>
    );
}
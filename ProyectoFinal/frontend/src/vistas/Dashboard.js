import { useState, useEffect } from 'react';
import { Cpu, HardDrive, LogOut, User, Eye, Shield, Activity, Mouse, Keyboard, Monitor } from 'lucide-react';

export default function Dashboard() {
    const [usoCpu, setUsoCpu] = useState(0);
    const [cpuNoUsado, setCpuNoUsado] = useState(0);
    const [usoRam, setUsoRam] = useState(0);
    const [ramNoUsada, setRamNoUsada] = useState(0);
    const [nombreUsuario, setNombreUsuario] = useState('Usuario');
    const [grupoUsuario, setGrupoUsuario] = useState('remote_control');
    const [error, setError] = useState(null);
    const [imagenPantalla, setImagenPantalla] = useState(null);
    const [ultimaActualizacion, setUltimaActualizacion] = useState(null);
    const [resolucion, setResolucion] = useState({ ancho: 1920, alto: 1080 });

    useEffect(() => {
        const datosUsuario = {
            usuario: 'Usuario Demo',
            grupo: 'remote_control' // Cambia a 'view_only' para probar sin control
        };
        setNombreUsuario(datosUsuario.usuario);
        setGrupoUsuario(datosUsuario.grupo);
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
                const respuesta = await fetch('http://localhost:8081/resolucion');
                const datos = await respuesta.json();
                if (!datos.error) {
                    setResolucion({ ancho: datos.ancho, alto: datos.alto });
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
                const respuesta = await fetch('http://localhost:8081/pantalla');
                if (respuesta.ok) {
                    const blob = await respuesta.blob();
                    const urlImagen = await ppmToPng(blob);
                    setImagenPantalla(urlImagen);
                    setUltimaActualizacion(new Date().toLocaleTimeString());
                } else {
                    console.error('Error al obtener pantalla:', respuesta.status);
                }
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
                const respuesta = await fetch('http://localhost:8081/cpu');
                const datos = await respuesta.json();
                if (datos.error) {
                    setError(datos.error);
                } else {
                    setUsoCpu(datos.porcentaje_usado);
                    setCpuNoUsado(datos.porcentaje_no_usado);
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
                const respuesta = await fetch('http://localhost:8081/ram');
                const datos = await respuesta.json();
                if (datos.error) {
                    setError(datos.error);
                } else {
                    setUsoRam(datos.porcentaje_usado);
                    setRamNoUsada(datos.porcentaje_no_usado);
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
        alert('Sesión cerrada');
    };

    const manejarClickPantalla = (e) => {
        if (tieneControlTotal) {
            const rect = e.currentTarget.getBoundingClientRect();
            const x = ((e.clientX - rect.left) / rect.width) * 100;
            const y = ((e.clientY - rect.top) / rect.height) * 100;
            console.log(`Click en pantalla: ${x.toFixed(2)}%, ${y.toFixed(2)}%`);
            // Aquí puedes enviar las coordenadas al backend
        }
    };
    const tieneControlTotal = grupoUsuario === 'remote_control';
    return (
        <div className="min-h-screen bg-slate-50">
        
            {/* ========== HEADER ========== */}
            <header className="bg-white border-b border-slate-200 shadow-sm">
                <div className="max-w-7xl mx-auto px-6 py-4">
                
                    {/* Fila única con todo */}
                    <div className="flex justify-between items-center">
                        
                        {/* Logo */}
                        <div className="flex items-center gap-3">
                            <div className="bg-slate-900 p-2 rounded-lg">
                                <Shield className="w-6 h-6 text-white" />
                            </div>
                            <div>
                                <h1 className="text-xl font-bold text-slate-900">USACLinux</h1>
                                <p className="text-xs text-slate-500">Escritorio Remoto</p>
                            </div>
                        </div>

                        {/* Métricas CPU y RAM */}
                        <div className="flex items-center gap-6">
                            
                            {/* CPU */}
                            <div className="flex items-center gap-3 bg-blue-50 px-4 py-2 rounded-lg border border-blue-200">
                                <div className="bg-blue-700 p-2 rounded-lg">
                                    <Cpu className="w-5 h-5 text-white" />
                                </div>
                                <div>
                                    <div className="text-xs text-slate-600 mb-1">CPU</div>
                                    <div className="flex items-center gap-3">
                                        <div className="text-center">
                                            <div className="text-lg font-bold text-slate-900">{usoCpu}%</div>
                                            <div className="text-xs text-slate-500">Usado</div>
                                        </div>
                                        <div className="w-px h-8 bg-slate-300"></div>
                                        <div className="text-center">
                                            <div className="text-lg font-bold text-emerald-600">{cpuNoUsado}%</div>
                                            <div className="text-xs text-slate-500">Libre</div>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            {/* RAM */}
                            <div className="flex items-center gap-3 bg-purple-50 px-4 py-2 rounded-lg border border-purple-200">
                                <div className="bg-purple-700 p-2 rounded-lg">
                                    <HardDrive className="w-5 h-5 text-white" />
                                </div>
                                <div>
                                    <div className="text-xs text-slate-600 mb-1">RAM</div>
                                    <div className="flex items-center gap-3">
                                        <div className="text-center">
                                            <div className="text-lg font-bold text-slate-900">{usoRam}%</div>
                                            <div className="text-xs text-slate-500">Usado</div>
                                        </div>
                                        <div className="w-px h-8 bg-slate-300"></div>
                                        <div className="text-center">
                                            <div className="text-lg font-bold text-emerald-600">{ramNoUsada}%</div>
                                            <div className="text-xs text-slate-500">Libre</div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>

                        {/* Usuario y Logout */}
                        <div className="flex items-center gap-4">
                            {/* Última actualización */}
                            {ultimaActualizacion && (
                                <div className="flex items-center gap-2 bg-slate-100 px-4 py-2 rounded-lg border border-slate-200">
                                    <Activity className="w-4 h-4 text-slate-600" />
                                    <div>
                                        <div className="text-xs text-slate-500">Última actualización</div>
                                        <div className="text-sm font-semibold text-slate-900">{ultimaActualizacion}</div>
                                    </div>
                                </div>
                            )}

                            <div className="flex items-center gap-2 bg-slate-100 px-4 py-2 rounded-lg border border-slate-200">
                                <User className="w-4 h-4 text-slate-600" />
                                <span className="font-semibold text-slate-900">{nombreUsuario}</span>
                            </div>

                            <button 
                                onClick={manejarCerrarSesion}
                                className="flex items-center gap-2 bg-slate-900 hover:bg-slate-800 text-white px-4 py-2 rounded-lg transition-colors"
                            >
                                <LogOut className="w-4 h-4" />
                                <span className="font-semibold">Salir</span>
                            </button>
                        </div>
                    </div>

                    {/* Mensaje de error si existe */}
                    {error && (
                        <div className="mt-4 bg-red-50 border border-red-200 rounded-lg p-3">
                            <p className="text-sm text-red-700">⚠️ {error}</p>
                        </div>
                    )}
                </div>
            </header>

            {/* ========== CONTENIDO PRINCIPAL ========== */}
            <main className="max-w-7xl mx-auto px-6 py-6">
                
                {/* Escritorio Remoto */}
                <div className="bg-white rounded-xl shadow-lg border border-slate-200 overflow-hidden">
                
                    {/* Título */}
                    <div className="bg-slate-900 px-6 py-4 flex justify-between items-center">
                        <h2 className="text-lg font-bold text-white">Escritorio Remoto</h2>
                        <div className="flex items-center gap-2">
                            <div className="w-2 h-2 bg-green-400 rounded-full animate-pulse"></div>
                            <span className="text-sm text-slate-300">Activo</span>
                        </div>
                    </div>

                    {/* Área de la imagen del escritorio */}
                    <div className="bg-slate-900 p-6 flex justify-center items-center">
                        <div 
                            className={`bg-slate-800 rounded-lg overflow-hidden border-2 border-slate-700 relative ${tieneControlTotal ? 'cursor-crosshair' : 'cursor-not-allowed'}`}
                            style={{ 
                                maxWidth: '100%',
                                maxHeight: '70vh',
                                aspectRatio: `${resolucion.ancho} / ${resolucion.alto}`
                            }}
                            onClick={manejarClickPantalla}
                        >
                            {imagenPantalla ? (
                                <img 
                                    src={imagenPantalla} 
                                    alt="Escritorio remoto"
                                    className="w-full h-full object-cover"
                                />
                            ) : (
                                <div className="w-full h-full flex items-center justify-center">
                                    <div className="text-center">
                                        <Activity className="w-12 h-12 text-slate-600 mx-auto mb-2 animate-pulse" />
                                        <p className="text-slate-400">Cargando escritorio...</p>
                                    </div>
                                </div>
                            )}
                            
                            {/* Overlay cuando no tiene control */}
                            {!tieneControlTotal && (
                                <div className="absolute inset-0 bg-slate-900 bg-opacity-30 flex items-center justify-center pointer-events-none">
                                    <div className="bg-blue-600 text-white px-4 py-2 rounded-lg flex items-center gap-2">
                                        <Eye className="w-5 h-5" />
                                        <span className="font-semibold">Solo Vista</span>
                                    </div>
                                </div>
                            )}
                        </div>
                    </div>

                    {/* Footer con permisos */}
                    <div className={`px-6 py-4 ${tieneControlTotal ? 'bg-emerald-50 border-t border-emerald-200' : 'bg-blue-50 border-t border-blue-200'}`}>
                        <div className="flex items-center justify-between">
                            <div className="flex items-center gap-3">
                                {tieneControlTotal ? (
                                    <>
                                        <div className="bg-emerald-600 p-2 rounded-lg">
                                            <Shield className="w-5 h-5 text-white" />
                                        </div>
                                        <div>
                                            <h3 className="font-semibold text-slate-900">Acceso Total</h3>
                                            <p className="text-sm text-slate-600">Puedes ver y controlar el escritorio</p>
                                        </div>
                                    </>
                                ) : (
                                    <>
                                        <div className="bg-blue-600 p-2 rounded-lg">
                                            <Eye className="w-5 h-5 text-white" />
                                        </div>
                                        <div>
                                            <h3 className="font-semibold text-slate-900">Solo Vista</h3>
                                            <p className="text-sm text-slate-600">Puedes ver el escritorio sin controlarlo</p>
                                        </div>
                                    </>
                                )}
                            </div>

                            {/* Iconos de permisos */}
                            <div className="flex gap-2">
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-slate-100 border-slate-300 text-slate-400'}`}>
                                    <Mouse className="w-4 h-4" />
                                    <span className="text-sm font-semibold">Mouse</span>
                                </div>
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-slate-100 border-slate-300 text-slate-400'}`}>
                                    <Keyboard className="w-4 h-4" />
                                    <span className="text-sm font-semibold">Teclado</span>
                                </div>
                                <div className={`flex items-center gap-2 px-4 py-2 rounded-lg border transition-all ${tieneControlTotal ? 'bg-white border-emerald-300 text-emerald-700' : 'bg-slate-100 border-slate-300 text-slate-400'}`}>
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
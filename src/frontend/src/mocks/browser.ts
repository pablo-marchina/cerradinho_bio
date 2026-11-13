import { setupWorker } from 'msw/browser';
import { http, HttpResponse, delay } from 'msw';
import { createSnapshot } from './fixtures';
import {registerAsset,updateAlert,startCommand,step,completeCommand,simulate} from '../services/simulator';
import type { AlertStatus,CommandInput,Scenario } from '../services/types';
let snapshot=createSnapshot();
const error=(e:unknown)=>HttpResponse.json({message:e instanceof Error?e.message:'Falha simulada'},{status:409});
export const worker=setupWorker(
 http.get('/api/demo/snapshot',async()=>{await delay(120);return HttpResponse.json(snapshot);}),
 http.patch('/api/demo/alerts/:id',async({params,request})=>{try{const b=await request.json() as {status:AlertStatus;reason:string};return HttpResponse.json(updateAlert(snapshot,String(params.id),b.status,b.reason));}catch(e){return error(e);}}),
 http.post('/api/demo/assets',async({request})=>{try{return HttpResponse.json(registerAsset(snapshot,await request.json() as Record<string,string>));}catch(e){return error(e);}}),
 http.post('/api/demo/commands',async({request})=>{try{const input=await request.json() as CommandInput;const current=snapshot,c=startCommand(current,input);step(current,c,'published');setTimeout(()=>{if(snapshot===current)completeCommand(current,c);},input.scenario==='timeout'?2500:900);return HttpResponse.json(c,{status:202});}catch(e){return error(e);}}),
 http.post('/api/demo/scenario',async({request})=>{try{const b=await request.json() as {id:string;scenario:Scenario};return HttpResponse.json(simulate(snapshot,b.id,b.scenario));}catch(e){return error(e);}}),
 http.post('/api/demo/reset',()=>{snapshot=createSnapshot();return HttpResponse.json({ok:true});})
);

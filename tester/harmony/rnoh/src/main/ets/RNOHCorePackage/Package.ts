import { RNPackage, TurboModulesFactory } from '../RNOH/RNPackage';
import type { TurboModule, TurboModuleContext } from '../RNOH/TurboModule';
import {
  AppStateTurboModule,
  DeviceEventManagerTurboModule,
  DeviceInfoTurboModule,
  ExceptionsManagerTurboModule,
  ImageLoaderTurboModule,
  NativeAnimatedTurboModule,
  NetworkingTurboModule,
  PlatformConstantsTurboModule,
  SourceCodeTurboModule,
  StatusBarTurboModule,
  TimingTurboModule,
  WebSocketTurboModule,
} from './turboModules';
import { LinkingManagerTurboModule } from './turboModules/LinkingManagerTurboModule';

export class RNOHCorePackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new CoreTurboModulesFactory(ctx);
  }
}

const TURBO_MODULE_CLASS_BY_NAME: Record<string, typeof TurboModule> = {
  [AppStateTurboModule.NAME]: AppStateTurboModule,
  [DeviceEventManagerTurboModule.NAME]: DeviceEventManagerTurboModule,
  [DeviceInfoTurboModule.NAME]: DeviceInfoTurboModule,
  [ExceptionsManagerTurboModule.NAME]: ExceptionsManagerTurboModule,
  [ImageLoaderTurboModule.NAME]: ImageLoaderTurboModule,
  [NativeAnimatedTurboModule.NAME]: NativeAnimatedTurboModule,
  [LinkingManagerTurboModule.NAME]: LinkingManagerTurboModule,
  [NetworkingTurboModule.NAME]: NetworkingTurboModule,
  [PlatformConstantsTurboModule.NAME]: PlatformConstantsTurboModule,
  [SourceCodeTurboModule.NAME]: SourceCodeTurboModule,
  [StatusBarTurboModule.NAME]: StatusBarTurboModule,
  [TimingTurboModule.NAME]: TimingTurboModule,
  [WebSocketTurboModule.NAME]: WebSocketTurboModule,
};

class CoreTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule {
    if (this.hasTurboModule(name)) {
      return new TURBO_MODULE_CLASS_BY_NAME[name](this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name in TURBO_MODULE_CLASS_BY_NAME;
  }
}

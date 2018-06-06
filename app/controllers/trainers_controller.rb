class TrainersController < ApplicationController
  skip_before_action :authenticate_user!

  def index
    @trainers = Trainer.all
  end

  def show
    @trainer = Trainer.find(params[:id])
    @expertise = Expertise.find(@trainer.expertise_id)
    # @expertise = Expertise.find(@trainer[:expertise_id])
  end

  def new
    @user = current_user
    @trainer = Trainer.new
    @trainer.user = @user
  end

  def create
    @user = current_user
    @trainer = Trainer.new(trainer_params)
    @trainer.user = @user
    @hourly_rate = params[:trainer][:hourly_rate].to_i
    @trainer.hourly_rate = @hourly_rate
    if @trainer.save
      redirect_to trainer_path(@trainer)
    else
      render :new
    end
    @expertise = Expertise.all
  end

  def edit
    @trainer = Trainer.find(params[:id])

  end

  def update
    # @trainer = Trainer.find(params[:id])
    # @trainer.update(params[:trainer_params])
  end

  def destroy
    # @trainer = Trainer.find(paramas[:id])
    # @trainer.destroy
  end



  private
  def trainer_params
    params.require(:trainer).permit(:expertise_id,:hourly_rate)
  end
end



